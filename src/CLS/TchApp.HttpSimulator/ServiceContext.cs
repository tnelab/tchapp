// Copyright (c) .NET Foundation. All rights reserved.
// Licensed under the Apache License, Version 2.0. See License.txt in the project root for license information.

using Microsoft.AspNetCore.Hosting;

namespace TchApp.HttpSimulator
{
    public class ServiceContext
    {
        public ServiceContext() { }
        public ServiceContext(ServiceContext context)
        {
            AppLifetime = context.AppLifetime;
        }

        public IApplicationLifetime AppLifetime { get; set; }
    }
}
