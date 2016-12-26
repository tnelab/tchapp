using System;
using System.Runtime.InteropServices;
using TchApp.TchClient;
using System.Reflection;

namespace tchclient
{
    // This project can output the Class library as a NuGet Package.
    // To enable this option, right-click on the project and select the Properties menu item. In the Build tab select "Produce outputs on build".
    public class Program
    {
        public static int Main(string[] args)
        {
            int exit_code = 0;
            exit_code = Application.This
                .AddResourceAssembly("tchclient")
                .Run("ui/index.html");                
            return exit_code;
        }
    }
}
