#ifndef CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_RESOURCE_HANDLER_H_
#define CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_RESOURCE_HANDLER_H_
#pragma once
#include "include/cef_resource_handler.h"
#include <functional>

namespace Tnelab {
	class TchResourceHandler :public CefResourceHandler
	{
	public:
		//拦截所有资源请求，过滤出所有http和https协议，把request数据转到用户代码进行处理	
		typedef int(*ResourceResponseDelegate)(TchResourceHandler* tch_resource_handler, int status_code, const char* status_text, const char* mime_type, const char* header_map, const char* response_body_datas, int response_body_datas_size);
		typedef int(*ResourceRequestDelegate)(TchResourceHandler* tch_resource_handler, const char* request_datas, int size, ResourceResponseDelegate delegate);


		friend class TchRequestResourceProvider;
		TchResourceHandler();
		virtual void Cancel() OVERRIDE;
		virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
			CefRefPtr<CefCallback> callback)
			OVERRIDE;
		virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
			int64& response_length,
			CefString& redirectUrl) OVERRIDE;
		virtual bool ReadResponse(void* data_out,
			int bytes_to_read,
			int& bytes_read,
			CefRefPtr<CefCallback> callback)
			OVERRIDE;
		virtual bool CanSetCookie(const CefCookie& cookie) OVERRIDE;
		virtual bool CanGetCookie(const CefCookie& cookie) OVERRIDE;

		//设置用户自动request委托
		static void SetResourceRequestDelegate(ResourceRequestDelegate delegate);
	private:
		CefResponse::HeaderMap header_map_;
		CefRefPtr<CefStreamReader> stream_;		
		int status_code_=200;
		std::string status_text_="";
		std::string mime_type_="";

		char* body_bytes_ = 0;
		int body_bytes_lenght_ =0;
		int body_bytes_offset_ =0;

		//把request对象转成字节流
		std::string DumpRequestContents(std::string url, std::string schemes, CefRefPtr<CefRequest> request);
		//void OnResourceResponse(const char* response_datas, int size);
		//资源处理委托
		static ResourceRequestDelegate user_resource_request_handle_;

		~TchResourceHandler() {
			if (body_bytes_ != 0) {
				delete[] body_bytes_;
				body_bytes_ = 0;
			}
		}

		IMPLEMENT_REFCOUNTING(TchResourceHandler);
	};
}
#endif  // CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_RESOURCE_HANDLER_H_
