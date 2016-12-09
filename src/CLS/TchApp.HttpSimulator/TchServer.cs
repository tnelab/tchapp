using System;
using System.Collections.Generic;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Hosting.Server;
using Microsoft.AspNetCore.Hosting.Server.Features;
using Microsoft.AspNetCore.Http.Features;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using System.Threading.Tasks;
using System.Text;
using TchApp.TchClient;

namespace TchApp.HttpSimulator
{
    public class TchServer : IServer
    {


        private Stack<IDisposable> _disposables;
        private readonly IApplicationLifetime _applicationLifetime;
        //private readonly ILogger _logger;
        private readonly IServerAddressesFeature _serverAddresses;
        public TchServer(IOptions<TchServerOptions> options, IApplicationLifetime applicationLifetime, ILoggerFactory loggerFactory)
        {
            if (options == null)
            {
                throw new ArgumentNullException(nameof(options));
            }

            if (applicationLifetime == null)
            {
                throw new ArgumentNullException(nameof(applicationLifetime));
            }

            if (loggerFactory == null)
            {
                throw new ArgumentNullException(nameof(loggerFactory));
            }

            Options = options.Value ?? new TchServerOptions();
            _applicationLifetime = applicationLifetime;
            //_logger = loggerFactory.CreateLogger(typeof(TchServer).GetTypeInfo().Assembly.FullName);
            Features = new FeatureCollection();
            //var componentFactory = new HttpComponentFactory(Options);
            //Features.Set<IHttpComponentFactory>(componentFactory);
            _serverAddresses = new ServerAddressesFeature();
            Features.Set<IServerAddressesFeature>(_serverAddresses);
        }
        public TchServerOptions Options { get; protected set; }
        public IFeatureCollection Features { get; protected set; }

        public void Dispose()
        {
            if (_disposables != null)
            {
                while (_disposables.Count > 0)
                {
                    _disposables.Pop().Dispose();
                }
                _disposables = null;
            }
        }

        public void Start<TContext>(IHttpApplication<TContext> application)
        {
            if (_disposables != null)
            {
                // The server has already started and/or has not been cleaned up yet
                throw new InvalidOperationException("Server has already started.");
            }
            _disposables = new Stack<IDisposable>();

            try
            {
                //var url = Options.StartUrl == null ? "http://tchapp" : Options.StartUrl;
                TchListener<TContext> listener = new TchListener<TContext>(application);
                listener.StartAsync(Options).Wait();
            }
            catch
            {
                Dispose();
                throw;
            }
        }
        public static void Start(Action action,bool isSimulator=true,string start_url="")
        {            
            var task = new Task(() => {
                action.Invoke();
            });
            task.Start();
            if (!isSimulator)
            {
                Application.This.Run(start_url);
            }
            else
            {
                task.Wait();
            }
        }
    }
}
