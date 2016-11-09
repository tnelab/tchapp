// TCHApp.Core.cpp : 定义 DLL 应用程序的导出函数。
//

#include "include/cef_sandbox_win.h"
#include "cefclient/browser/main_message_loop_multithreaded_win.h"

#include "TchClientExport.h"
#include <string>
#include "include/base/cef_logging.h"
#include "include/base/cef_scoped_ptr.h"
#include "include/cef_app.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"
#include "cefclient/browser/client_app_browser.h"
#include "cefclient/browser/main_context_impl.h"
#include "cefclient/browser/main_message_loop_std.h"
#include "cefclient/browser/test_runner.h"
#include "cefclient/common/client_app_other.h"



using namespace client;

int TchStart(const char* url, bool sizeable, int x,int y,int width,int height) {
	// Enable High-DPI support on Windows 7 or newer.
	CefEnableHighDPISupport();
	CefMainArgs main_args(::GetModuleHandle(0));
	void* sandbox_info = NULL;

#if defined(CEF_USE_SANDBOX)
	// Manage the life span of the sandbox information object. This is necessary
	// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#endif
	// Parse command-line arguments.
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();	
	std::wstringstream command_str;
	//set command line string

	//command_str << L"TchApp";
	command_str << L" --multi-threaded-message-loop=true";
	//command_str << L" --cache-path=./TchApp.CefClient/cache";
	command_str << L" --url=";
	command_str << url;
	//command_str<<L" off-screen-rendering-enabled=true";
	//command_str<<L" off-screen-frame-rate=15";
	//command_str<<L" transparent-painting-enabled=true";
	command_str << L" --show-update-rect=true";
	//command_str<<L" --mouse-cursor-change-disabled=true";
	//command_str << L" --request-context-per-browser=true";
	command_str << L" --request-context-shared-cache=true";
	//command_str<<L" --background-color=#ffffff";
	command_str << L" --enable-gpu=true";
	//command_str<<L" --filter-url=http://www.baidu.com,http://www.sina.com.cn";

	command_line->InitFromString(command_str.str());
	// Create a ClientApp of the correct type.
	CefRefPtr<CefApp> app;
	app = new ClientAppBrowser();

	// Execute the secondary process, if any.
	int exit_code = CefExecuteProcess(main_args, app, sandbox_info);
	if (exit_code >= 0)
		return exit_code;

	// Create the main context object.
	scoped_ptr<MainContextImpl> context(new MainContextImpl(command_line, true));

	CefSettings settings;

#if !defined(CEF_USE_SANDBOX)
	settings.no_sandbox = true;
#endif
	wchar_t cef_full_path[MAX_PATH];
	GetModuleFileName(GetModuleHandle(L"libcef.dll"), cef_full_path, MAX_PATH);
	std::wstring cef_full_path_str = cef_full_path;
	auto scan_index= cef_full_path_str.rfind('\\');
	auto sub_exe_path = cef_full_path_str.substr(0, scan_index).append(L"\\tchsubprocess.exe");

	CefString(&settings.browser_subprocess_path).FromWString(sub_exe_path);
	CefString(&settings.log_file).FromWString(L"cef_log.txt");
	// Populate the settings based on command line arguments.
	context->PopulateSettings(&settings);
	//set single process
	//settings.single_process = true;

	//CefString(&settings.log_file).FromASCII("./TchApp.CefClient/log_file.txt");

	// Create the main message loop object.
	scoped_ptr<MainMessageLoop> message_loop;

	if (settings.multi_threaded_message_loop)
		message_loop.reset(new MainMessageLoopMultithreadedWin);
	else
		message_loop.reset(new MainMessageLoopStd);

	// Initialize CEF.
	context->Initialize(main_args, settings, app, sandbox_info);
	// Register scheme handlers.
	test_runner::RegisterSchemeHandlers();
	
	//zmg 2016-11-6
	/*
	// Create the first window.
	context->GetRootWindowManager()->CreateRootWindow(
		true,             // Show controls.
		settings.windowless_rendering_enabled ? true : false,
		CefRect(),        // Use default system size.
		std::string(url));   // Use default URL.
	*/
	//zmg
	settings.windowless_rendering_enabled = true;
	CefRect rect;
	rect.x = x;
	rect.y = y;
	rect.width = width;
	rect.height = height;
	context->GetRootWindowManager()->CreateRootWindow(
		false,             // Show controls.
		settings.windowless_rendering_enabled ? true : false,
		rect,        // Use default system size.
		std::string(url),
		sizeable);   // Use default URL.
	//zmg end

						  // Run the message loop. This will block until Quit() is called by the
						  // RootWindowManager after all windows have been destroyed.
	int result = message_loop->Run();

	// Shut down CEF.
	context->Shutdown();
	
	// Release objects in reverse order of creation.
	message_loop.reset();
	context.reset();

	return result;
}