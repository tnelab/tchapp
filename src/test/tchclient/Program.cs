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
                .UseTchAppDomainName("www.8888.com")
                //.Run("ui/doraemon.html");
                .Run("ui/index.html");                
                //.Run("http://tests/other_tests");
                //.Run("http://image.baidu.com/search/detail?ct=503316480&z=0&ipn=d&word=gif&step_word=&hs=0&pn=0&spn=0&di=111381035702&pi=0&rn=1&tn=baiduimagedetail&is=0%2C0&istype=0&ie=utf-8&oe=utf-8&in=&cl=2&lm=-1&st=undefined&cs=2749190246%2C3857616763&os=2333236995%2C3810477213&simid=4197481872%2C851121125&adpicid=0&ln=1985&fr=&fmq=1481386154935_R&fm=&ic=undefined&s=undefined&se=&sme=&tab=0&width=&height=&face=undefined&ist=&jit=&cg=&bdtype=0&oriquery=&objurl=http%3A%2F%2Fhd.shijue.cvidea.cn%2Ftf%2F140826%2F2348436%2F53fc92f23dfae91c56000005.GIF&fromurl=ippr_z2C%24qAzdH3FAzdH3Ffit37j_z%26e3B4jAzdH3Fvwpj256tjfAzdH3Ft1jwAzdH3F9uuwjluvb11ub09ukbaanj8w%3Frw2j%3D8b&gsm=0&rpstart=0&rpnum=0");
            return exit_code;
        }
    }
}
