// Copyright (c) .NET Foundation. All rights reserved.
// Licensed under the Apache License, Version 2.0. See License.txt in the project root for license information.

using System;
using System.Collections.Generic;
using Microsoft.Extensions.Configuration;

namespace TchApp.HttpSimulator
{
    public class TchServerOptions
    {
        public IServiceProvider ApplicationServices { get; set; }
        public string StartUrl { get; set; }
        public string WindowSettingQueryKey { get; set; }
        public string BlockedDomain { get; set; }
    }
}
