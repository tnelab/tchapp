#include "TchClientExport.h"
int SetTchErrorDelegate(TchErrorHandler::TchErrorDelegate* delegate) {
	Tnelab::TchErrorHandler::SetTchErrorDelegate((TchErrorHandler::TchErrorDelegate)delegate);
	return 0;
}
void SetJsInvokeDelegate(TchQueryHandler::JsInvokeDelegate delegate) {
	TchQueryHandler::SetJsInvokeDelegate(delegate);
}
void SetResourceRequestDelegate(TchResourceHandler::ResourceRequestDelegate delegate) {
	TchResourceHandler::SetResourceRequestDelegate(delegate);
}
void OnTchError(int code, const char *msg) {
	TchErrorHandler::OnTchError(code, std::string(msg));
}