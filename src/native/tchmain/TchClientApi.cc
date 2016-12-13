#include <string>
#include "TchMainExport.h"

#ifdef OS_WIN
#include <Windows.h>
HMODULE htchclient = GetModuleHandle(0);
#define GetFuncPtr(h,l) GetProcAddress(h,l)
#else
#include <dlfcn.h>
#define GetFuncPtr(h,l) dlsym(h,l)
void* htchclient = dlopen(NULL, RTLD_NOW);
#endif // OS_WIN

int TchStart(const Tnelab::TchAppStartSettings start_settings) {
	auto func = (TchStart_ptr)GetFuncPtr(htchclient, "TchStart");
	return func(start_settings);
}
int SetTchErrorDelegate(void* delegate) {
	auto func = (SetTchErrorDelegate_ptr)GetFuncPtr(htchclient, "SetTchErrorDelegate");
	return func(delegate);
}
void SetJsInvokeDelegate(void* delegate) {
	auto func = (SetJsInvokeDelegate_ptr)GetFuncPtr(htchclient, "SetJsInvokeDelegate");
	func(delegate);
}
void SetResourceRequestDelegate(void* delegate) {
	auto func = (SetResourceRequestDelegate_ptr)GetFuncPtr(htchclient, "SetResourceRequestDelegate");
	func(delegate);
}
void OnTchError(int code,const char* msg) {
	auto func = (OnTchError_ptr)GetFuncPtr(htchclient, "OnTchError");
	func(code,msg);
}
void SetTchAppDomainName(const char* domain_name) {
	auto func = (SetTchAppDomainName_ptr)GetFuncPtr(htchclient, "SetTchAppDomainName");
	func(domain_name);
}