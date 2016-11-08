#include "TchErrorHandler.h"
#include <stdio.h>
#ifdef OS_WIN
#include <Windows.h> 
#endif // OS_WIN

namespace Tnelab{
	TchErrorHandler::TchErrorDelegate TchErrorHandler::_tch_error_delegate = 0;
	void TchErrorHandler::OnTchError(int error_code, std::string error_msg) {
		if (_tch_error_delegate != 0) {
			_tch_error_delegate(error_code, error_msg);
			return;
		}
#ifdef OS_WIN
		if (!error_msg.empty()) {
			char msg[1024] = { 0 };
			sprintf(msg, "TCH_ERROR:%d,%s\r\n", error_code, error_msg.c_str());
			AllocConsole();
			HANDLE hd = GetStdHandle(STD_OUTPUT_HANDLE);
			WriteConsoleA(hd, msg, strlen(msg), NULL, NULL);
			system("pause");
			CloseHandle(hd);
		}
#else
		if (!error_msg.empty())	printf("TCH_ERROR:%d,%s\r\n", error_code, error_msg.c_str());
#endif // OS_WIN				
	}
	int TchErrorHandler::SetTchErrorDelegate(TchErrorDelegate delegate) {
		_tch_error_delegate = delegate;
		return 0;
	}
}