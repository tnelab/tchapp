#ifndef CEF_TESTS_CEFCLIENT_TCH_ADD_EXPORT_H_
#define CEF_TESTS_CEFCLIENT_TCH_ADD_EXPORT_H_
#pragma once
#include "TchResourceHandler.h"
#include "TchQueryHandler.h"
#include "TchErrorHandler.h"

using namespace Tnelab;

#if defined(OS_LINUX)
#define TCHAPI  __attribute__((visibility("default"))) 
#elif defined(OS_WIN)
#define TCHAPI _declspec(dllexport)
#endif
//说明:启动CefClient
//url:启动时访问的url，如果为空将访问http://www.baidu.com
//on_resource_request_delegate:资源请求委托，所有拦截下来的请求都传送给on_resource_request_delegate
//on_js_invoke_delegate:js调用委托
//返回值:cefclient将返回退出标志,非0表示非正常退出
//调用例子:
//

extern "C"{
	TCHAPI int TchStart(const char* url, bool sizeable = true, int x = -1, int y = -1, int width = 800, int height = 600);
	TCHAPI int SetTchErrorDelegate(TchErrorHandler::TchErrorDelegate* delegate);
	TCHAPI void SetJsInvokeDelegate(TchQueryHandler::JsInvokeDelegate delegate);
	TCHAPI void SetResourceRequestDelegate(TchResourceHandler::ResourceRequestDelegate delegate);
	TCHAPI void OnTchError(int code, const char* msg);
}
#endif  // CEF_TESTS_CEFCLIENT_TCH_ADD_EXPORT_H_
