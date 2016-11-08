using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TchApp.TchClient
{
    public class TchRequestInfo
    {
        public string Method { get; set; }
        public Uri Uri { get; set; }
        public string HttpVersion { get; set; }
        public Dictionary<string, string> Headers { get; } = new Dictionary<string, string>();        
        public bool KeepAlive { get; set; } = false;
        public Stream Body { get; } = new MemoryStream();
        public byte[] RawBytes { get;set; }
    }
    public class TchResponseInfo
    {
        public int StatusCode { get; set; }
        public string StatusText { get; set; }
        public Dictionary<string, string> Headers { get; } = new Dictionary<string, string>();
        public string HeaderMap
        {
            get
            {
                StringBuilder builder = new StringBuilder();
                foreach (string key in this.Headers.Keys)
                {
                    builder.Append(key).Append(": ").AppendLine(this.Headers[key]);
                }
                return builder.ToString();
            }
        }
        public string MimeType { set; get; }
        public byte[] Body { get; set; }
    }
    public static class TchHelper
    {
        public static TchRequestInfo ParseToRequest(byte[] request_bytes)
        {
            TchRequestInfo request_info = new TchRequestInfo();
            request_info.RawBytes = request_bytes;
            string request_str = Encoding.UTF8.GetString(request_bytes);

            int current_index = 0;
            int scan_index = request_str.IndexOf(' ');
            request_info.Method = request_str.Substring(current_index, scan_index);
            current_index = scan_index + 1;

            scan_index = request_str.IndexOf(' ', current_index);
            request_info.Uri = new Uri(request_str.Substring(current_index, scan_index - current_index));
            current_index = scan_index + 1;

            scan_index = request_str.IndexOf('\r', current_index);
            request_info.HttpVersion = request_str.Substring(current_index, scan_index - current_index);
            current_index = scan_index + 2;

            string header_line;
            int header_scan_index = 0;
            while (current_index != request_str.Length && request_str[current_index] != '\r')
            {
                scan_index = request_str.IndexOf('\r', current_index);
                header_line = request_str.Substring(current_index, scan_index - current_index);
                if (header_line.IndexOf("Connection") == 0 && header_line.IndexOf("keep-alive") != -1) request_info.KeepAlive = true;
                header_scan_index = header_line.IndexOf(':');
                request_info.Headers.Add(header_line.Substring(0, header_scan_index), header_line.Substring(header_scan_index + 2, header_line.Length - header_scan_index - 2));
                current_index = scan_index + 2;
            }
            if (current_index < request_str.Length)
            {
                current_index += 2;
                string body_str = request_str.Substring(current_index, request_str.Length - current_index);
                byte[] body_bytes = Encoding.UTF8.GetBytes(body_str);
                request_info.Body.Write(body_bytes, 0, body_bytes.Length);
                request_info.Body.Flush();
                request_info.Body.Position = 0;
                if (!request_info.Headers.ContainsKey("Content-Length"))
                {
                    request_info.Headers.Add("Content-Length", body_bytes.Length.ToString());
                }
            }
            return request_info;
        }
        public static TchRequestInfo ParseToRequest(string request_str)
        {
            return ParseToRequest(Encoding.UTF8.GetBytes(request_str));
        }
        public static TchRequestInfo ParseToRequest(IntPtr request_ptr,int size)
        {
            byte[] bytes = new byte[size];
            for (var i = 0; i < size; ++i)
            {
                bytes[i] = Marshal.ReadByte(request_ptr + i);
            }
            return ParseToRequest(bytes);
        }
        public static TchResponseInfo ParseToResponse(byte[] body_bytes,int status_code=200,string status_text="OK", string mime_type = "text/html")
        {
            TchResponseInfo info = new TchResponseInfo();
            info.StatusCode = status_code;
            info.StatusText = status_text;
            info.MimeType = mime_type;
            StringBuilder header_builder = new StringBuilder();

            //info.Headers.Add("Connection", "keep-alive");
            info.Headers.Add("Encoding", "UTF-8");
            info.Headers.Add("Content-Length", body_bytes.Length.ToString());
            info.Headers.Add("Content-Type", mime_type+ "; charset=UTF-8");
            info.Body = body_bytes;
            return info;
        }
        public static TchResponseInfo ParseToResponse(string body_str)
        {
            return ParseToResponse(Encoding.UTF8.GetBytes(body_str));
        }        
    }
}
