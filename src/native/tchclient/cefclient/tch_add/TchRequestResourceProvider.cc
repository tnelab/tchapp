#include "TchRequestResourceProvider.h"
#include "include/cef_parser.h"
#include "include/cef_browser.h"
#include "include/cef_request.h"
#include "include/wrapper/cef_message_router.h"
#include "include/wrapper/cef_resource_manager.h"
#include "TchResourceHandler.h"
#include <iostream>
#include <fstream>

namespace Tnelab {	
	std::string TchRequestResourceProvider::blocked_domain_=std::string("tchapp.localhost");
	void TchRequestResourceProvider::SetBlockedDomain(const char* blocked_domain) {
		std::string blocked_domain_str = std::string(blocked_domain);
		if (!blocked_domain_str.empty())
		{
			transform(blocked_domain_str.begin(), blocked_domain_str.end(), blocked_domain_str.begin(), tolower);
			TchRequestResourceProvider::blocked_domain_ = blocked_domain_str;
		}
	}
	bool TchRequestResourceProvider::OnRequest(scoped_refptr<CefResourceManager::Request> request) {
		CEF_REQUIRE_IO_THREAD();
		if (TchResourceHandler::user_resource_request_handle_ == 0) return false;
		std::string url = request->request()->GetURL();
		std::string schemes = url.substr(0, url.find(':'));
		transform(schemes.begin(), schemes.end(), schemes.begin(), tolower);
		if (schemes != "http"&&schemes != "https") return false;
		std::string tchapp_sign = url.substr(0, TchRequestResourceProvider::blocked_domain_.length()+8);
		transform(tchapp_sign.begin(), tchapp_sign.end(), tchapp_sign.begin(), tolower);
		std::string blocked_sign;
		blocked_sign.append(schemes).append("://").append(TchRequestResourceProvider::blocked_domain_).append("/\0");		
		if (tchapp_sign.find(blocked_sign) != 0) return false;
		request->Continue(new TchResourceHandler());
		return true;
	}
}