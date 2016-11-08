//#pragma once
#ifndef TCHAPP_MAIN_EXPORT_H_
#define TCHAPP_MAIN_EXPORT_H_

#define CORECLR_HOSTING_API(function, ...) \
    typedef int (*function##_ptr)(__VA_ARGS__)

CORECLR_HOSTING_API(coreclr_initialize,
	const char* exePath,
	const char* appDomainFriendlyName,
	int propertyCount,
	const char** propertyKeys,
	const char** propertyValues,
	void** hostHandle,
	unsigned int* domainId);

CORECLR_HOSTING_API(coreclr_shutdown,
	void* hostHandle,
	unsigned int domainId);

CORECLR_HOSTING_API(coreclr_create_delegate,
	void* hostHandle,
	unsigned int domainId,
	const char* entryPointAssemblyName,
	const char* entryPointTypeName,
	const char* entryPointMethodName,
	void** delegate);

CORECLR_HOSTING_API(coreclr_execute_assembly,
	void* hostHandle,
	unsigned int domainId,
	int argc,
	const char** argv,
	const char* managedAssemblyPath,
	unsigned int* exitCode);

#undef CORECLR_HOSTING_API

#define TCH_CLIENT_API(function,return_type, ...) \
    typedef return_type (*function##_ptr)(__VA_ARGS__);

TCH_CLIENT_API(TchStart, int, const char* url, bool sizeable, int x,int y,int width,int height);
TCH_CLIENT_API(SetTchErrorDelegate, int, void* delegate);
TCH_CLIENT_API(SetJsInvokeDelegate, void, void* delegate);
TCH_CLIENT_API(SetResourceRequestDelegate, void, void* delegate);
TCH_CLIENT_API(OnTchError, void, int code, const char* msg);

#undef TCH_CLIENT_API


#if defined(OS_LINUX)
#define TCHAPI  __attribute__((visibility("default"))) 
#elif defined(OS_WIN)
#define TCHAPI _declspec(dllexport)
#endif
extern "C" {
	TCHAPI int TchMain(int argc, char* argv[]);
	TCHAPI int TchStart(const char* url, bool sizeable = true, int x = -1, int y = -1, int width = 800, int height = 600);
	TCHAPI int SetTchErrorDelegate(void* delegate);
	TCHAPI void SetJsInvokeDelegate(void* delegate);
	TCHAPI void SetResourceRequestDelegate(void* delegate);
	TCHAPI void OnTchError(int code, const char* msg);
}


#endif // !TCHAPP_MAIN_EXPORT_H_