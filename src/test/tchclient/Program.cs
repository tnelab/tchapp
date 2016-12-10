using System;
using System.Runtime.InteropServices;
using TchApp.TchClient;
using tchclient.ui;
using System.Reflection;

namespace tchclient
{
    // This project can output the Class library as a NuGet Package.
    // To enable this option, right-click on the project and select the Properties menu item. In the Build tab select "Produce outputs on build".
    public class Program
    {
        public static int Main(string[] args)
        {
            test test = new test();
            test.ExecuteAsync().Wait();
            var html = test.ToString();

            var tmp = Assembly.Load(new AssemblyName("tchclient"));
            var names = tmp.GetManifestResourceInfo("tchclient.ui.index.html");


            int exit_code = 0;
            exit_code = Application.This
                .AddResourceAssembly("tchclient")
                .UseTchAppDomainName("www.8888.com")
                .Run("http://img.zcool.cn/community/01515c55422dfc0000019ae9334d9c.jpg");

            return exit_code;
        }
    }
}