#include <string>
#include "TchMainExport.h"

#ifdef OS_WIN
#include <Windows.h>
HMODULE htchclient = GetModuleHandle(0);
#define GetFuncPtr(h,l) GetProcAddress(h,l)
#else
#include <dlfcn.h>
#define GetFuncPtr(h,l) dlsym(h,l)
extern std::string exe_name;
void* htchclient = dlopen(exe_name.c_str(), RTLD_NOLOAD);
#endif // OS_WIN

int TchStart(const char* url, int x, int y, int width, int height) {
	auto func = (TchStart_ptr)GetFuncPtr(htchclient, "TchStart");
	return func(url,x,y,width,height);
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