using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Text;
using System.Resources;
using System.IO;
using System.Reflection;

namespace TchApp.TchClient
{
    /// <summary>
    /// 窗口启动位置
    /// </summary>
    public enum TchAppStartPosition {
        /// <summary>
        /// 手动设置
        /// </summary>
        Manual,
        /// <summary>
        /// 屏幕中心，忽略x,y设置
        /// </summary>
        CenterScreen
    };
    /// <summary>
    /// cef窗口默认构造参数
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]    
	public struct TchAppStartSettings
    {
        /// <summary>
        /// url
        /// </summary>
		public string Url;
        /// <summary>
        /// 窗口启动位置，参看枚举：TchAppStartPosition
        /// </summary>
		public TchAppStartPosition StartPosition;
        /// <summary>
        /// 窗口屏幕x坐标
        /// </summary>
		public int X;
        /// <summary>
        /// 窗口y坐标
        /// </summary>
		public int Y;
        /// <summary>
        /// 窗口宽度
        /// </summary>
		public int Width;
        /// <summary>
        /// 窗口高度
        /// </summary>
		public int Height;
	}
    /// <summary>
    /// 负责与web交互
    /// </summary>
    public sealed class Application
    {
        #region 原始c导出函数及委托
        [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
        delegate int ResourceResponseDelegate(IntPtr handler, int status_code, string status_text, string mime_type, string header_map, IntPtr response_body_datas, int response_body_datas_size);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        delegate int ResourceRequestDelegate(IntPtr handler, IntPtr request_datas, int size, ResourceResponseDelegate on_resource_response);

        [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        delegate string JsInvokeDelegate(string invoke_json);

        [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl,CharSet=CharSet.Ansi)]
        delegate void TchErrorDelegate(int error_code, string error_msg);

        [DllImport("tchmain")]
        extern static int TchStart(TchAppStartSettings start_settings);
        [DllImport("tchmain")]
        extern static int SetTchErrorDelegate(TchErrorDelegate func);
        [DllImport("tchmain")]
        extern static void SetJsInvokeDelegate(JsInvokeDelegate func);
        [DllImport("tchmain")]
        extern static void SetResourceRequestDelegate(ResourceRequestDelegate func);
        [DllImport("tchmain")]
        extern static void OnTchError(int code,string msg);
        [DllImport("tchmain")]
        extern static void SetTchAppDomainName(string domain_name);
        #endregion

        #region 导出c函数封装

        Func<string, string> processTchJsInvoke { get; set; }
        /// <summary>
        /// 默认JS调用处理函数
        /// </summary>
        /// <param name="msg">js传送过来的消息</param>
        /// <returns></returns>
        string jsInvokeProcessor(string msg)
        {
            return this.processTchJsInvoke(msg);
        }

        Action<int, string> processTchError { get; set; }
        /// <summary>
        /// 默认Tch处理函数
        /// </summary>
        /// <param name="code">错误代码</param>
        /// <param name="msg">错误消息</param>
        void tchErrorProcessor(int code, string msg)
        {
            this.processTchError(code, msg);
        }

        /// <summary>
        /// 原始web请求处理委托，web请求处理器的执行中心，调用方为cef3
        /// </summary>
        /// <param name="handler">cef3通信参数</param>
        /// <param name="request_datas">未解析的请求数据本机指针</param>
        /// <param name="size">数据大小</param>
        /// <param name="on_resource_response">回送处理结果委托</param>
        /// <returns></returns>
        int onTchRequest(IntPtr handler, IntPtr request_datas, int size, ResourceResponseDelegate on_resource_response)
        {
            TchResponseInfo response_info;
            try
            {
                TchRequestInfo tch_request_info = TchHelper.ParseToRequest(request_datas, size);
                response_info= this.tchRequestProcessor(tch_request_info);
            }
            catch(Exception ex)
            {
                Application.This.OnError(-1, $"request exception:{ex.Message}");
                response_info=TchHelper.ParseToResponse($"<html><head><meta charset=\"UTF-8\"/><title>处理请求错误</title></head><body><h1>处理请求发生异常，{ex.Message}</h1></body></html>");
            }
            try
            {
                //把请求结果回送本机到本机内存
                IntPtr response_datas_ptr = Marshal.AllocHGlobal(response_info.Body.Length);
                for (int i = 0; i < response_info.Body.Length; ++i)
                {
                    Marshal.WriteByte(response_datas_ptr + i, response_info.Body[i]);
                }
                //回送结果到cef3
                on_resource_response(handler, response_info.StatusCode, response_info.StatusText, response_info.MimeType, response_info.HeaderMap, response_datas_ptr, response_info.Body.Length);
                Marshal.FreeHGlobal(response_datas_ptr);
            }
            catch(Exception ex)
            {
                Application.This.OnError(-1, $"request exception:{ex.Message}");
            }
            return 0;
        }
        #endregion

        /// <summary>
        /// 单例构造函数
        /// </summary>
        private Application()
        {
            SetResourceRequestDelegate(onTchRequest);
            if (this.processTchJsInvoke != null) SetJsInvokeDelegate(this.jsInvokeProcessor);
            if (this.processTchError != null) SetTchErrorDelegate(this.tchErrorProcessor);
            this.AddResourceAssembly("TchApp.TchClient");
            this.UseTchRequestProcessor(DefaultTchRequestProcessor.This.ProcessTchRequest);
        }

        #region API扩展和公共接口
        /// <summary>
        /// 单例client对象
        /// </summary>
        public static Application This = new Application();

        public int Run(TchAppStartSettings start_settings)
        {
            string url = start_settings.Url;
            Uri result;
            if (!Uri.TryCreate(url, UriKind.Absolute, out result))
                if (!Uri.TryCreate($"http://{this.TchAppDomainName}/{url}", UriKind.Absolute, out result))
                    if (!Uri.TryCreate($"http://{this.TchAppDomainName}{url}", UriKind.Absolute, out result))
                        throw new ArgumentException($"[{url}] is bad url.");
            start_settings.Url = result.AbsoluteUri;
            int exit_code = TchStart(start_settings);
            return exit_code;
        }

        /// <summary>
        /// 启动web解析和渲染
        /// </summary>
        /// <param name="url">初始url,tch默认内部域名为：tchapp.localhost,eg:http://tchapp.localhost/ui/index.html or ui/index.html or /ui/index.html</param>
        /// <param name="start_position">窗口是启动位置，参看枚举：TchAppStartPosition</param>
        /// <param name="width">窗口初始宽度</param>
        /// <param name="height">窗口初始高度</param>
        /// <param name="border_width">窗口边框大小</param>
        /// <param name="x">窗口初始x坐标</param>
        /// <param name="y">窗口初始y坐标</param>
        /// <returns>无措返回0</returns>        
        public int Run(string url, int width = 800, int height = 600,TchAppStartPosition start_position = TchAppStartPosition.CenterScreen, int x=0,int y=0)
        {
            TchAppStartSettings start_settings;
            start_settings.Url = url;
            start_settings.StartPosition = start_position;
            start_settings.X = x;
            start_settings.Y = y;
            start_settings.Width = width;
            start_settings.Height = height;
            int exit_code= Run(start_settings);
            return exit_code;
        }

        /// <summary>
        /// 引发一个给错误给错误处理器统一处理
        /// </summary>
        /// <param name="code">错误码</param>
        /// <param name="msg">错误消息</param>
        public void OnError(int code,string msg)
        {
            OnTchError(code, msg);
        }

        internal readonly HashSet<Assembly> ResourceAssemblySet = new HashSet<Assembly>();
        /// <summary>
        /// 添加包含web资源的程序集,这些程序集可以用于web请求时获取资源,请注意填写正确的程序集名称
        /// TchApp自带资源：jquery.3.1.1.min.js和TchApp.js
        /// 引用url:http://tchapp.localhost/jquery-3.1.1.min.js和http://tchapp.localhost/tchapp.js
        /// </summary>
        /// <param name="assembly_name">包含资源的程序集名称</param>
        /// <returns>Client对象</returns>
        public Application AddResourceAssembly(string assembly_name)
        {            
            ResourceAssemblySet.Add(Assembly.Load(new AssemblyName(assembly_name)));
            return this;
        }

        /// <summary>
        /// 设置js调用处理器，用于处理来自js的调用
        /// </summary>
        /// <param name="tch_js_invoke_processor">js调用处理器</param>
        /// <returns>Client对象</returns>
        public Application UseTchJsInvokeProcessor(Func<string, string> tch_js_invoke_processor) {
            this.processTchJsInvoke = tch_js_invoke_processor;
            return this;
        }

        /// <summary>
        /// 设置tch 错误处理器，用于统一处理错误
        /// </summary>
        /// <param name="tch_error_processor">错误处理器</param>
        /// <returns>Client对象</returns>
        public Application UseTchErrorProcessor(Action<int, string> tch_error_processor)
        {
            this.processTchError = tch_error_processor;
            return this;
        }

        Func<TchRequestInfo, TchResponseInfo> tchRequestProcessor { get; set; }
        /// <summary>
        /// 自定义处理web请求，请求通过TchResponseInfo送回web，
        /// 为确保处理每个请求，请在Start方法执行前调用本方法，
        /// TchApp.TchClient.Helper类中包含一些http协议解析辅助方法,
        /// 调用默认处理器可以使用TchApp.TchClient.DefaultTchRequestProcessor.This对象
        /// 默认处理顺序:
        /// 1.如果url不带扩展名，查找url文件指向的cshtml类 
        /// 2.本地查找同名文件
        /// 3.查找资源中同名资源
        /// </summary>        
        /// <param name="tch_request_processor">请求处理器</param>
        /// <returns>Client对象</returns>
        public Application UseTchRequestProcessor(Func<TchRequestInfo,TchResponseInfo> tch_request_processor)
        {
            this.tchRequestProcessor = tch_request_processor;
            return this;
        }

        internal readonly Dictionary<string, Func<string,TchResponseInfo>> FilterUrlDic = new Dictionary<string, Func<string, TchResponseInfo>>();
        /// <summary>
        /// 添加url 过滤器
        /// </summary>
        /// <param name="url">需要过滤的url eg:/ui/index.html</param>
        /// <param name="url_filter">能够返回url指向资源的处理器</param>
        /// <returns>url指向的资源字节数组</returns>
        public Application AddUrlFilter(string url,Func<string,TchResponseInfo> url_filter)
        {
            this.FilterUrlDic.Add(url, url_filter);
            return this;
        }

        string tchapp_domain_name_ = "tchapp.localhost";
        /// <summary>
        /// 获取当前tchapp的转发域名，向此域名发出的请求都会被转发给程序本身，而不是寻找该域名所在的服务器。
        /// 该域名默认为tchapp.localhost，该域名必须符合互联网域名命名规则。
        /// </summary>
        public string TchAppDomainName
        {
            set
            {
                Uri result;
                bool is_ok = Uri.TryCreate("http://" + value + "/", UriKind.Absolute, out result);
                if (!is_ok)
                    throw new ArgumentException($"[{value}] is bad domain name.");
                this.tchapp_domain_name_ = result.Host;
                SetTchAppDomainName(tchapp_domain_name_);
            }
            get
            {
                return this.tchapp_domain_name_;
            }
        }
        /// <summary>
        /// 设置tchapp的转发域名，向此域名发出的请求都会被转发给程序本身，而不是寻找该域名所在的服务器。
        /// 该域名默认为tchapp.localhost，该域名必须符合互联网域名命名规则。
        /// </summary>
        /// <param name="domain_name">域名</param>
        /// <returns></returns>
        public Application UseTchAppDomainName(string domain_name)
        {
            this.TchAppDomainName = domain_name;
            return this;
        }
        #endregion
    }
}
