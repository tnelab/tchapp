#include <gtk/gtk.h>
#include <gtk/gtkgl.h>

#include <X11/Xlib.h>
#undef Success     // Definition conflicts with cef_message_router.h
#undef RootWindow  // Definition conflicts with root_window.h

#include <stdlib.h>
#include <unistd.h>
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
#include "cefclient/common/client_switches.h"
#include <linux/limits.h>
#include <unistd.h>

#include "TchClientExport.h"
using namespace client;

int XErrorHandlerImpl(Display *display, XErrorEvent *event) {
	LOG(WARNING)
		<< "X error received: "
		<< "type " << event->type << ", "
		<< "serial " << event->serial << ", "
		<< "error_code " << static_cast<int>(event->error_code) << ", "
		<< "request_code " << static_cast<int>(event->request_code) << ", "
		<< "minor_code " << static_cast<int>(event->minor_code);
	return 0;
}

int XIOErrorHandlerImpl(Display *display) {
	return 0;
}

void TerminationSignalHandler(int signatl) {
	LOG(ERROR) << "Received termination signal: " << signatl;
	MainContext::Get()->GetRootWindowManager()->CloseAllWindows(true);
}

int cef_argc=0;
char* cef_argv[16];
int TchStart(const TchAppStartSettings start_settings) {
	TchWindowApi::StartSettings = start_settings;
	// Create a copy of |argv| on Linux because Chromium mangles the value
	// internally (see issue #620).	

	int argc=cef_argc;	
	char* argv[16];
	for (int i = 0; i < argc; ++i) argv[i] = cef_argv[i];

	// Create a copy of |argv| on Linux because Chromium mangles the value
	// internally (see issue #620).
	CefScopedArgArray scoped_arg_array(argc, argv);
	char** argv_copy = scoped_arg_array.array();


	CefMainArgs main_args(argc, argv);


	//std::stringstream command_str;
	//set command line string

	//command_str << L"TchApp";
	//command_str << L" --multi-threaded-message-loop";
	//command_str << L" --cache-path=./TchApp.CefClient/cache";
	//command_str << L" --url=";
	//command_str << url;
	//command_str << L" --off-screen-rendering-enabled";
	//command_str<<L" --off-screen-frame-rate=15";
	//command_str << L" --transparent-painting-enabled";
	//command_str << L" --show-update-rect";
	//command_str<<L" --mouse-cursor-change-disabled";
	//command_str << L" --request-context-per-browser";
	//command_str << L" --request-context-shared-cache";
	//command_str<<L" --background-color=#ffffff";
	//command_str << L" --enable-gpu";
	//command_str << L" --disable-gpu";
	//command_str << L" --disable-gpu-compositing";
	//command_str<<L" --filter-url=http://www.baidu.com,http://www.sina.com.cn";
	//command_str<<L" --type=renderer";//进程类型:windows为renderer，linux为zygote
	//command_str<<L" --use-views";
	//command_str<<L" --thide-frame";//进程类型:windows为renderer，linux为zygote
	//command_str << L" --hide-controls";

    int cmd_argc=6;
	char* cmd_argv[cmd_argc];
	cmd_argv[0]=const_cast<char*>("--url=https://wwww.baidu.com");
	cmd_argv[1]=const_cast<char*>("--off-screen-rendering-enabled");
	cmd_argv[2]=const_cast<char*>("--request-context-shared-cache");
	cmd_argv[3]=const_cast<char*>("--enable-gpu");
	cmd_argv[4]=const_cast<char*>("--transparent-painting-enabled");
	cmd_argv[5]=const_cast<char*>("--hide-controls");

	// Parse command-line arguments.
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
	command_line->InitFromArgv(cmd_argc, cmd_argv);

	// Create a ClientApp of the correct type.
	CefRefPtr<CefApp> app;
 	app = new ClientAppBrowser();
	
	// Execute the secondary process, if any.
	int exit_code = CefExecuteProcess(main_args, app, NULL);
	if (exit_code >= 0)
		return exit_code;

	// Create the main context object.
	scoped_ptr<MainContextImpl> context(new MainContextImpl(command_line, true));

	CefSettings settings;

	char exe_full_path[PATH_MAX];
	char link[PATH_MAX];
	sprintf(link,"/proc/%d/exe", getpid());
	auto i = readlink(link, exe_full_path, sizeof(exe_full_path));
	exe_full_path[i] = '\0';
	std::string exe_full_path_str = exe_full_path;
	auto scan_index = exe_full_path_str.rfind('/');
	auto sub_exe_path = exe_full_path_str.substr(0, scan_index).append("/tchsubprocess");
	CefString(&settings.browser_subprocess_path).FromString(sub_exe_path);
	CefString(&settings.log_file).FromWString(L"cef_log.txt");

	// Populate the settings based on command line arguments.
	context->PopulateSettings(&settings);
	//set single process
	//settings.single_process = true;

	// Create the main message loop object.
	scoped_ptr<MainMessageLoop> message_loop(new MainMessageLoopStd);

	// Initialize CEF.
	context->Initialize(main_args, settings, app, NULL);
	
	// The Chromium sandbox requires that there only be a single thread during
	// initialization. Therefore initialize GTK after CEF.
	gtk_init(&argc, &argv_copy);

	// Perform gtkglext initialization required by the OSR example.
	gtk_gl_init(&argc, &argv_copy);

	// Install xlib error handlers so that the application won't be terminated
	// on non-fatal errors. Must be done after initializing GTK.
	XSetErrorHandler(XErrorHandlerImpl);
	XSetIOErrorHandler(XIOErrorHandlerImpl);

	// Install a signal handler so we clean up after ourselves.
	signal(SIGINT, TerminationSignalHandler);
	signal(SIGTERM, TerminationSignalHandler);

	// Register scheme handlers.
	test_runner::RegisterSchemeHandlers();

	// Create the first window.
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