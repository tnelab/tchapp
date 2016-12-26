#ifndef CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_REQUEST_RESOURCE_PROVIDER_H_
#define CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_REQUEST_RESOURCE_PROVIDER_H_
#pragma once
#include "include/cef_browser.h"
#include "include/cef_request.h"
#include "include/wrapper/cef_message_router.h"
#include "include/wrapper/cef_resource_manager.h"
#include <functional>
namespace Tnelab {	
	// Provider that dumps the request contents.
	//拦截所有资源请求，过滤出所有http和https协议，把request数据转到用户代码进行处理
	class TchRequestResourceProvider : public CefResourceManager::Provider {
	public:
		explicit TchRequestResourceProvider() {}

		bool OnRequest(scoped_refptr<CefResourceManager::Request> request) OVERRIDE;
		static void SetBlockedDomain(const char* blocked_domain);	
		static std::string GetBlockDomain();

	private:
		static std::string blocked_domain_;
		DISALLOW_COPY_AND_ASSIGN(TchRequestResourceProvider);
	};	
}
#endif  // CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_REQUEST_RESOURCE_PROVIDER_H_