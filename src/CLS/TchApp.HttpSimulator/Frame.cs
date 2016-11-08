using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Text;
using Microsoft.AspNetCore.Http;
using System.Threading;
using Microsoft.AspNetCore.Hosting.Server;
using TchApp.TchClient;

namespace TchApp.HttpSimulator
{    
    internal partial class Frame:IDisposable
    {
        private static readonly ArraySegment<byte> _emptyData = new ArraySegment<byte>(new byte[0]);

        private readonly object _onStartingSync = new Object();
        private readonly object _onCompletedSync = new Object();

        //private bool _corruptedRequest = false;
        //private Headers _frameHeaders;
        //private Streams _frameStreams;

        private List<KeyValuePair<Func<object, Task>, object>> _onStarting;

        private List<KeyValuePair<Func<object, Task>, object>> _onCompleted;

        //private bool _requestProcessingStarted;
        //private Task _requestProcessingTask;
        //private volatile bool _requestProcessingStopping; // volatile, see: https://msdn.microsoft.com/en-us/library/x13ttww7.aspx
        private int _requestAborted;
        private CancellationTokenSource _abortedCts;
        private CancellationToken? _manuallySetRequestAbortToken;

        private bool _responseStarted;
        private bool _keepAlive;
        //private bool _autoChunk;
        //private Exception _applicationException;

        private readonly MemoryStream _responseStream = new MemoryStream();
        private enum HttpVersionType
        {
            Unknown = -1,
            Http1_0 = 0,
            Http1_1 = 1
        }
        private HttpVersionType _httpVersion;

        //private readonly string _pathBase;


        public Frame(byte[] request_bytes)
        {
            Init_();
            ParseRequest_(request_bytes);
        }

        private string Scheme { get; set; }
        private string Method { get; set; }
        private string RequestUri { get; set; }
        private string PathBase { get; set; }
        private string Path { get; set; }
        private string QueryString { get; set; }
        private string RawTarget { get; set; }
        private string HttpVersion
        {
            get
            {
                if (_httpVersion == HttpVersionType.Http1_1)
                {
                    return "HTTP/1.1";
                }
                if (_httpVersion == HttpVersionType.Http1_0)
                {
                    return "HTTP/1.0";
                }
                return "";
            }
            set
            {
                if (value == "HTTP/1.1")
                {
                    _httpVersion = HttpVersionType.Http1_1;
                }
                else if (value == "HTTP/1.0")
                {
                    _httpVersion = HttpVersionType.Http1_0;
                }
                else
                {
                    _httpVersion = HttpVersionType.Unknown;
                }
            }
        }
        private IHeaderDictionary RequestHeaders { get; set; }
        private Stream RequestBody { get; set; }

        private int StatusCode { get; set; }
        private string ReasonPhrase { get; set; }
        private IHeaderDictionary ResponseHeaders { get; set; }
        private Stream ResponseBody { get; set; }

        private Stream DuplexStream { get; set; }


        private CancellationToken RequestAborted
        {
            get
            {
                // If a request abort token was previously explicitly set, return it.
                if (_manuallySetRequestAbortToken.HasValue)
                {
                    return _manuallySetRequestAbortToken.Value;
                }
                // Otherwise, get the abort CTS.  If we have one, which would mean that someone previously
                // asked for the RequestAborted token, simply return its token.  If we don't,
                // check to see whether we've already aborted, in which case just return an
                // already canceled token.  Finally, force a source into existence if we still
                // don't have one, and return its token.
                var cts = _abortedCts;
                return
                    cts != null ? cts.Token :
                    (Volatile.Read(ref _requestAborted) == 1) ? new CancellationToken(true) :
                    RequestAbortedSource.Token;
            }
            set
            {
                // Set an abort token, overriding one we create internally.  This setter and associated
                // field exist purely to support IHttpRequestLifetimeFeature.set_RequestAborted.
                _manuallySetRequestAbortToken = value;
            }
        }

        private CancellationTokenSource RequestAbortedSource
        {
            get
            {
                // Get the abort token, lazily-initializing it if necessary.
                // Make sure it's canceled if an abort request already came in.
                var cts = LazyInitializer.EnsureInitialized(ref _abortedCts, () => new CancellationTokenSource());
                if (Volatile.Read(ref _requestAborted) == 1)
                {
                    cts.Cancel();
                }
                return cts;
            }
        }
        public bool HasResponseStarted
        {
            get { return _responseStarted; }
        }

        private void OnStarting(Func<object, Task> callback, object state)
        {
            lock (_onStartingSync)
            {
                if (_onStarting == null)
                {
                    _onStarting = new List<KeyValuePair<Func<object, Task>, object>>();
                }
                _onStarting.Add(new KeyValuePair<Func<object, Task>, object>(callback, state));
            }
        }

        private void OnCompleted(Func<object, Task> callback, object state)
        {
            lock (_onCompletedSync)
            {
                if (_onCompleted == null)
                {
                    _onCompleted = new List<KeyValuePair<Func<object, Task>, object>>();
                }
                _onCompleted.Add(new KeyValuePair<Func<object, Task>, object>(callback, state));
            }
        }

        private async Task FireOnStarting()
        {
            List<KeyValuePair<Func<object, Task>, object>> onStarting = null;
            lock (_onStartingSync)
            {
                onStarting = _onStarting;
                _onStarting = null;
            }
            if (onStarting != null)
            {
                try
                {
                    foreach (var entry in onStarting)
                    {
                        await entry.Key.Invoke(entry.Value);
                    }
                }
                catch (Exception ex)
                {
                    //ReportApplicationError(ex);
                    throw ex;
                }
            }
        }

        private async Task FireOnCompleted()
        {
            List<KeyValuePair<Func<object, Task>, object>> onCompleted = null;
            lock (_onCompletedSync)
            {
                onCompleted = _onCompleted;
                _onCompleted = null;
            }
            if (onCompleted != null)
            {
                foreach (var entry in onCompleted)
                {
                    try
                    {
                        await entry.Key.Invoke(entry.Value);
                    }
                    catch (Exception ex)
                    {
                        //ReportApplicationError(ex);
                        throw ex;
                    }
                }
            }
        }
        private void Flush()
        {
            //ProduceStartAndFireOnStarting().GetAwaiter().GetResult();
            //SocketOutput.Write(_emptyData);
        }

        private async Task FlushAsync(CancellationToken cancellationToken)
        {
            //await ProduceStartAndFireOnStarting();
            //await SocketOutput.WriteAsync(_emptyData, cancellationToken: cancellationToken);
        }
        /// <summary>
        /// Immediate kill the connection and poison the request and response streams.
        /// </summary>
        private void Abort()
        {
            //if (Interlocked.CompareExchange(ref _requestAborted, 1, 0) == 0)
            //{
            //    _requestProcessingStopping = true;

            //    _frameStreams?.RequestBody.Abort();
            //    _frameStreams?.ResponseBody.Abort();

            //    try
            //    {
            //        ConnectionControl.End(ProduceEndType.SocketDisconnect);
            //    }
            //    catch (Exception ex)
            //    {
            //        Log.LogError(0, ex, "Abort");
            //    }

            //    try
            //    {
            //        RequestAbortedSource.Cancel();
            //    }
            //    catch (Exception ex)
            //    {
            //        Log.LogError(0, ex, "Abort");
            //    }
            //    _abortedCts = null;
            //}
        }
        private void Init_()
        {
            _onStarting = null;
            _onCompleted = null;

            _responseStarted = false;
            _keepAlive = true;
            //_autoChunk = false;
            //_applicationException = null;

            //_requestHeaders.Reset();
            //ResetResponseHeaders();
            ResetFeatureCollection();

            Method = null;
            RequestUri = null;
            Path = null;
            QueryString = null;
            HttpVersion = null;
            RequestHeaders = new FrameRequestHeaders();
            //MessageBody = null;
            RequestBody = new MemoryStream();
            StatusCode = 200;
            ReasonPhrase = null;
            ResponseHeaders = new FrameResponseHeaders();
            ResponseBody = _responseStream;
            DuplexStream = new FrameDuplexStream(RequestBody, ResponseBody);

        }
        public TchResponseInfo GetResponseInfo()
        {
            this.FireOnStarting().Wait();
            this.FireOnCompleted().Wait();

            var info = new TchResponseInfo();
            info.StatusCode = this.StatusCode;
            info.StatusText = ReasonPhrases.ToStatus(StatusCode, ReasonPhrase);

            var hasConnection = false;
            var hasTransferEncoding = false;
            var hasContentLength = false;
            foreach (var header in this.ResponseHeaders)
            {
                var isConnection = false;
                if (!hasConnection &&
                    string.Equals(header.Key, "Connection", StringComparison.OrdinalIgnoreCase))
                {
                    hasConnection = isConnection = true;
                }
                else if (!hasTransferEncoding &&
                    string.Equals(header.Key, "Transfer-Encoding", StringComparison.OrdinalIgnoreCase))
                {
                    hasTransferEncoding = true;
                }
                else if (!hasContentLength &&
                    string.Equals(header.Key, "Content-Length", StringComparison.OrdinalIgnoreCase))
                {
                    hasContentLength = true;
                }

                foreach (var value in header.Value)
                {
                    info.Headers.Add(header.Key, value);
                    if (isConnection && value.IndexOf("close", StringComparison.OrdinalIgnoreCase) != -1)
                    {
                        _keepAlive = false;
                    }
                }


            }
            //if (_keepAlive && !hasTransferEncoding && !hasContentLength)
            //{
            //    if (HttpVersion == "HTTP/1.1")
            //    {
            //        _autoChunk = true;
            //        response_builder.Append("Transfer-Encoding: chunked\r\n");
            //    }
            //    else
            //    {
            //        _keepAlive = false;
            //    }
            //}
            info.Headers.Add("Content-Length", this._responseStream.Length.ToString());
            if (_keepAlive == false && hasConnection == false && HttpVersion == "HTTP/1.1")
            {
                info.Headers.Add("Connection","close");
            }
            else if (_keepAlive && hasConnection == false && HttpVersion == "HTTP/1.0")
            {
                info.Headers.Add("Connection","keep-alive");
            }

            if (this.ResponseHeaders.ContainsKey("Content-Type"))
            {
                info.MimeType = this.ResponseHeaders["Content-Type"];
                int i = info.MimeType.IndexOf(';');
                if(i>-1)info.MimeType = info.MimeType.Substring(0, i);
            }
            else
            {
                info.MimeType = "text/html";
            }
            byte[] body_bytes=new byte[0];
            if (this._responseStream.Length > 0)
            {
                body_bytes = new byte[this._responseStream.Length];
                this._responseStream.Position = 0;
                this._responseStream.Read(body_bytes, 0, body_bytes.Length);
            }
            info.Body = body_bytes;
            return info;
        }
    
        private void ParseRequest_(byte[] request_bytes)
        {
            string request_str = Encoding.UTF8.GetString(request_bytes);

            int current_index = 0;
            int scan_index=request_str.IndexOf(' ');
            this.Method = request_str.Substring(current_index, scan_index);
            current_index = scan_index+1;

            scan_index = request_str.IndexOf(' ', current_index);
            this.RequestUri = request_str.Substring(current_index,scan_index-current_index);
            Uri uri = new Uri(this.RequestUri);
            this.Scheme = uri.Scheme;
            this.Path = uri.LocalPath;
            this.PathBase = "";
            this.QueryString = uri.Query;
            current_index = scan_index+1;

            scan_index = request_str.IndexOf('\r',current_index);
            this.HttpVersion = request_str.Substring(current_index, scan_index - current_index);
            current_index = scan_index + 2;

            string header_line;
            int header_scan_index = 0;
            while (current_index != request_str.Length && request_str[current_index] != '\r')
            {
                scan_index = request_str.IndexOf('\r',current_index);
                header_line = request_str.Substring(current_index, scan_index - current_index);
                if (header_line.IndexOf("Connection") == 0 && header_line.IndexOf("keep-alive") != -1) _keepAlive = true;
                 header_scan_index= header_line.IndexOf(':');
                this.RequestHeaders.Add(header_line.Substring(0,header_scan_index),header_line.Substring(header_scan_index+2,header_line.Length-header_scan_index-2));
                current_index = scan_index + 2;
            }
            if (current_index < request_str.Length)
            {
                current_index += 2;
                string body_str = request_str.Substring(current_index, request_str.Length - current_index);
                byte[] body_bytes = Encoding.UTF8.GetBytes(body_str);
                this.RequestBody.Write(body_bytes, 0, body_bytes.Length);
                this.RequestBody.Flush();
                this.RequestBody.Position = 0;
                if(!RequestHeaders.ContainsKey("Content-Length"))
                {
                    RequestHeaders.Add("Content-Length", body_bytes.Length.ToString());
                }
            }
            _responseStarted = true;
        }

        #region IDisposable Support
        private bool disposedValue = false; // 要检测冗余调用

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    // TODO: 释放托管状态(托管对象)。
                }

                // TODO: 释放未托管的资源(未托管的对象)并在以下内容中替代终结器。
                // TODO: 将大型字段设置为 null。
                this.ResponseBody.Dispose();
                this.RequestBody.Dispose();
                disposedValue = true;
            }
        }

        // TODO: 仅当以上 Dispose(bool disposing) 拥有用于释放未托管资源的代码时才替代终结器。
        ~Frame()
        {
            // 请勿更改此代码。将清理代码放入以上 Dispose(bool disposing) 中。
            Dispose(false);
        }

        // 添加此代码以正确实现可处置模式。
        public void Dispose()
        {
            // 请勿更改此代码。将清理代码放入以上 Dispose(bool disposing) 中。
            Dispose(true);
            // TODO: 如果在以上内容中替代了终结器，则取消注释以下行。
            // GC.SuppressFinalize(this);
        }
        #endregion
    }
}
