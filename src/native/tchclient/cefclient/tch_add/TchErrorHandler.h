#pragma once
#ifndef CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_ERROR_HANDLER_H
#define CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_ERROR_HANDLER_H
#include <string>
namespace Tnelab {	
	class TchErrorHandler {
	public:
		typedef void(*TchErrorDelegate)(int error_code, std::string error_msg);

		static void OnTchError(int error_code, std::string error_msg);
		static int SetTchErrorDelegate(TchErrorDelegate delegate);
	private:
		static TchErrorDelegate _tch_error_delegate;
	};	
}
#endif //CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_ERROR_HANDLER_H