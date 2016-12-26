#ifndef CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_QUERY_HANDLER_H_
#define CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_QUERY_HANDLER_H_
#pragma once
#include "include/wrapper/cef_helpers.h"
#include "include/cef_parser.h"
#include "include/wrapper/cef_message_router.h"
#include <set>
#include <functional>

namespace Tnelab {	
	// Handle messages in the browser process.
	//处理来自js的调用,调用格式为cefQuery(request:request_body,onSuccess: success_callback,onFailure:failure_callback);
	//request_body:{"TchQuery":func_name,"Arguments":[arg1,arg2...]}
	//sucess_callback=function(msg){}
	//failure_callback=function(error_code,error_info){}
	class TchQueryHandler : public CefMessageRouterBrowserSide::Handler {
	public:			
		//处理js调用
		typedef wchar_t* (*JsInvokeDelegate)(const char16* invoke_json);
		TchQueryHandler();
		~TchQueryHandler();

		// Called due to cefQuery execution in binding.html.
		virtual bool OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) OVERRIDE;
		//设置用户Js调用委托
		static void SetJsInvokeDelegate(JsInvokeDelegate delegate);
	private:
		//资源处理委托
		static JsInvokeDelegate user_js_invoke_handle_;
		std::map<std::string, std::function<bool(CefRefPtr<CefDictionaryValue> request_dic, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback)>> query_processor_map_;
		
		void InitQueryProcessorMap_();
		
		//TchQueryProcessor List
		//处理JsInvok
		bool JsInvokeProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback);
		//处理SetCaptionRect
		bool SetCaptionRectProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback);
		//处理CloseWindow
		bool CloseWindowProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback);
		//处理MinimizeWindow
		bool MinimizeWindowProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback);
		//处理MaximizingWindow
		bool MaximizingWindowProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback);
		//处理RestoreWindow
		bool RestoreWindowProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback);
		//处理HideWindow
		bool HideWindowProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback);
		//处理ShowWindow
		bool ShowWindowProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback);
		//处理SetWindowBorder
		bool SetWindowBorderProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback);
		//处理SetWindowPos
		bool SetWindowPosProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback);
		//处理GetWindowPos
		bool GetWindowPosProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback);
		//处理GetTchAppDomainName
		bool GetTchAppDomainNameProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback);
	};
}
#endif  // CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_QUERY_HANDLER_H_
