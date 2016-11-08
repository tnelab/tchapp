using Microsoft.AspNetCore.Hosting.Server;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using TchApp.TchClient;

namespace TchApp.HttpSimulator
{
    internal class TchListener<TContext>
    {
        private IHttpApplication<TContext> applicaition_;
        private TchResponseInfo _OnResourceRequest(TchRequestInfo request_info)
        {
            try
            {
                using (Frame frame = new Frame(request_info.RawBytes))
                {
                    var context = applicaition_.CreateContext(frame);
                    applicaition_.ProcessRequestAsync(context).Wait();
                    var response_info = frame.GetResponseInfo();
                    return response_info;                    
                }
            }
            catch (Exception ex)
            {
                TchResponseInfo tch_response_info = new TchResponseInfo();
                tch_response_info.StatusCode = 200;
                tch_response_info.StatusText = ReasonPhrases.ToStatusPhrase(tch_response_info.StatusCode);
                tch_response_info.MimeType = "text/html";
                tch_response_info.Body = Encoding.UTF8.GetBytes(ex.GetLast().Message);
                tch_response_info.Headers.Add("Content-Type", "text/html; charset=utf-8");
                tch_response_info.Headers.Add("Content-Length",tch_response_info.Body.Length.ToString());
                return tch_response_info;
            }
        }
        public TchListener(IHttpApplication<TContext> applicaition){
            this.applicaition_ = applicaition;
        }
        public async Task<int> StartAsync(TchServerOptions options)
        {
            if (options.BlockedDomain == null) options.BlockedDomain = "tchapp";
            if (options.StartUrl == null) options.StartUrl = "http://tchapp";

            var task = new Task<int>(() => {
                var argv=System.Environment.GetCommandLineArgs();
                return Client.This
                .UseTchRequestProcessor(_OnResourceRequest)
                .Start(options.StartUrl);
            });
            task.Start();
            await task;
            return task.Result;
        }
    }
}
