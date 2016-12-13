// TCHApp.Core.cpp : 定义 DLL 应用程序的导出函数。
//

#include "TchClientExport.h"

#include <windows.h>
#include "include/base/cef_scoped_ptr.h"
#include "include/cef_command_line.h"
#include "include/cef_sandbox_win.h"
#include "cefclient/browser/client_app_browser.h"
#include "cefclient/browser/main_context_impl.h"
#include "cefclient/browser/main_message_loop_multithreaded_win.h"
#include "cefclient/browser/main_message_loop_std.h"
#include "cefclient/browser/root_window_manager.h"
#include "cefclient/browser/test_runner.h"
#include "cefclient/common/client_app_other.h"
#include "cefclient/common/client_switches.h"
#include <string>

using namespace client;
using namespace Tnelab;

int TchStart(const TchAppStartSettings start_settings) {
	TchWindowApi::StartSettings = start_settings;
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

	std::wstringstream command_str;
	//set command line string
	//command_str << L"TchApp";
	//command_str << L" --multi-threaded-message-loop";
	//command_str << L" --cache-path=./TchApp.CefClient/cache";
	command_str << L" --url=https://www.baidu.com";
	command_str<<" --off-screen-rendering-enabled";
	//command_str<<L" --off-screen-frame-rate=15";
	command_str<< L" --transparent-painting-enabled";
	command_str << L" --show-update-rect";
	//command_str<<L" --mouse-cursor-change-disabled";
	//command_str << L" --request-context-per-browser";
	command_str << L" --request-context-shared-cache";
	command_str<<L" --background-color=#ff0000";
	command_str << L" --enable-gpu";
	//command_str << L" --disable-gpu";
	//command_str << L" --disable-gpu-compositing";
	//command_str<<L" --filter-url=http://www.baidu.com,http://www.sina.com.cn";
	//command_str<<L" --type=renderer";//进程类型:windows为renderer，linux为zygote
	command_str<<L" --use-views";
	//command_str<<L" --thide-frame";
	command_str<< L" --hide-controls";


	// Parse command-line arguments.
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
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
	
	//zmg 2016-11-10 
	/*
	// Create the first window.
	context->GetRootWindowManager()->CreateRootWindow(
		!command_line->HasSwitch(switches::kHideControls),             // Show controls.
		settings.windowless_rendering_enabled ? true : false,
		CefRect(),        // Use default system size.
		std::string(url));   // Use default URL.
	*/
	//zmg
	CefRect rect;
	rect.x = TchWindowApi::StartSettings.X;
	rect.y = TchWindowApi::StartSettings.Y;
	rect.width = TchWindowApi::StartSettings.Width;
	rect.height = TchWindowApi::StartSettings.Height;
	context->GetRootWindowManager()->CreateRootWindow(
		!command_line->HasSwitch(switches::kHideControls),             // Show controls.
		settings.windowless_rendering_enabled ? true : false,
		rect,        // Use default system size.
		TchWindowApi::StartSettings.Url);   // Use default URL.
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