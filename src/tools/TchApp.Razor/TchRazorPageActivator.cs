using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Reflection;
using System.IO;

namespace TchApp.Razor
{
    public class TchRazorActivator
    {
        public static readonly string RootNamespace="TchApp.RazorPages";
        public static Assembly InnerAssembly;
        public static IRazorPage GetPage(ViewContext context, string page_path)
        {
            if (context == null)
            {
                throw new ArgumentNullException(nameof(context));
            }
            if (string.IsNullOrEmpty(context.ExecutingFilePath))
            {
                throw new InvalidOperationException(nameof(context.ExecutingFilePath));
            }
            if (!context.ExecutingFilePath.StartsWith("/"))
            {
                throw new ArgumentException($"{context.ExecutingFilePath}必须是以/开头的绝对路.");
            }
            if (string.IsNullOrEmpty(page_path))
            {
                throw new ArgumentNullException(nameof(page_path));
            }
            if (InnerAssembly == null)
            {
                throw new InvalidOperationException("TchRazorPageActivator.InnerAssembly is null");
            }
            var executing_file_path = context.ExecutingFilePath.Replace('/', Path.DirectorySeparatorChar);
            page_path = page_path.Replace('/', Path.DirectorySeparatorChar);
            if (!page_path.StartsWith("/"))
            {                
                page_path =Path.Combine(Path.GetDirectoryName(executing_file_path),page_path);
            }
            if (!page_path.EndsWith(".cshtml", StringComparison.OrdinalIgnoreCase))
                page_path+=".cshtml";
            string page_type_name = RootNamespace+"."+Path.GetFileNameWithoutExtension(page_path.Remove(0, 1).Replace(Path.DirectorySeparatorChar, '.'));
            Type page_type = InnerAssembly.GetType(page_type_name);
            if (page_type == null)
                return null;
            var page=Activator.CreateInstance(page_type) as IRazorPage;
            page.ViewContext = context;
            page.Path = page_path.Replace(Path.DirectorySeparatorChar,'/');
            return page;
        }
        public static List<IRazorPage> GetViewStartList(ViewContext context)
        {
            if (context == null)
            {
                throw new ArgumentNullException(nameof(context));
            }
            if (string.IsNullOrEmpty(context.ExecutingFilePath))
            {
                throw new InvalidOperationException(nameof(context.ExecutingFilePath));
            }
            List<IRazorPage> view_start_list = new List<IRazorPage>();

            string[] dirs= context.ExecutingFilePath.Split('/');
            string dir = "";
            for(int i = 0; i < dirs.Length-1;++i)
            {
                dir += dirs[i]+"/";
                var page = GetPage(context, dir+"_ViewStart.cshtml");
                if (page != null)
                {
                    view_start_list.Add(page);
                }
            }
            return view_start_list;
        }
        public static IView GetView(ViewContext context,string page_path)
        {
            var page_obj = GetPage(context,page_path);
            IView view = new TchRazorView(GetViewStartList(context), page_obj);
            context.View = view;
            return view;
        }
    }
}
