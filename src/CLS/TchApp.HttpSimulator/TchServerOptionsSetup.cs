using Microsoft.Extensions.Options;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace TchApp.HttpSimulator
{
    public class TchServerOptionsSetup : IConfigureOptions<TchServerOptions>
    {
        private IServiceProvider _services;

        public TchServerOptionsSetup(IServiceProvider services)
        {
            _services = services;
        }

        public void Configure(TchServerOptions options)
        {
            options.ApplicationServices = _services;
        }
    }
}
