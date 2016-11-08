// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//zmg 2016-6-1
/*
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
#include "cefclient/renderer/client_app_renderer.h"

namespace client {
namespace {

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

int RunMain(int argc, char* argv[]) {
  // Create a copy of |argv| on Linux because Chromium mangles the value
  // internally (see issue #620).
  CefScopedArgArray scoped_arg_array(argc, argv);
  char** argv_copy = scoped_arg_array.array();

  CefMainArgs main_args(argc, argv);

  // Parse command-line arguments.
  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
  command_line->InitFromArgv(argc, argv);

  // Create a ClientApp of the correct type.
  CefRefPtr<CefApp> app;
  ClientApp::ProcessType process_type = ClientApp::GetProcessType(command_line);
  if (process_type == ClientApp::BrowserProcess) {
    app = new ClientAppBrowser();
  } else if (process_type == ClientApp::RendererProcess ||
             process_type == ClientApp::ZygoteProcess) {
    // On Linux the zygote process is used to spawn other process types. Since
    // we don't know what type of process it will be give it the renderer
    // client.
    app = new ClientAppRenderer();
  } else if (process_type == ClientApp::OtherProcess) {
    app = new ClientAppOther();
  }

  // Execute the secondary process, if any.
  int exit_code = CefExecuteProcess(main_args, app, NULL);
  if (exit_code >= 0)
    return exit_code;

  // Create the main context object.
  scoped_ptr<MainContextImpl> context(new MainContextImpl(command_line, true));

  CefSettings settings;

  // Populate the settings based on command line arguments.
  context->PopulateSettings(&settings);

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
  context->GetRootWindowManager()->CreateRootWindow(
      true,             // Show controls.
      settings.windowless_rendering_enabled ? true : false,
      CefRect(),        // Use default system size.
      std::string());   // Use default URL.

  // Run the message loop. This will block until Quit() is called.
  int result = message_loop->Run();

  // Shut down CEF.
  context->Shutdown();

  // Release objects in reverse order of creation.
  message_loop.reset();
  context.reset();

  return result;
}

}  // namespace
}  // namespace client


// Program entry point function.
int main(int argc, char* argv[]) {
  return client::RunMain(argc, argv);
}
*/
//zmg
// Program entry point function.

#include <iostream>
#include <string>
#include <locale>
#include <cstring>
#include <dlfcn.h>
#include "cefclient.h"

std::wstring s2ws(const std::string& s)
{
	std::locale sys_loc("");

	const char* src_str = s.c_str();
	const size_t BUFFER_SIZE = s.size() + 1;

	wchar_t* intern_buffer = new wchar_t[BUFFER_SIZE];
	std::wmemset(intern_buffer, 0, BUFFER_SIZE);

	const char* extern_from = src_str;
	const char* extern_from_end = extern_from + s.size();
	const char* extern_from_next = 0;
	wchar_t* intern_to = intern_buffer;
	wchar_t* intern_to_end = intern_to + BUFFER_SIZE;
	wchar_t* intern_to_next = 0;

	typedef std::codecvt<wchar_t, char, mbstate_t> CodecvtFacet;

	mbstate_t in_cvt_state;
	const std::codecvt<wchar_t, char, mbstate_t>& ct = std::use_facet<CodecvtFacet>(sys_loc);
	auto cvt_rst = ct.in(
		in_cvt_state,
		extern_from, extern_from_end, extern_from_next,
		intern_to, intern_to_end, intern_to_next);

	if (cvt_rst != CodecvtFacet::ok) {
		switch (cvt_rst) {
		case CodecvtFacet::partial:
			std::cerr << "partial";
			break;
		case CodecvtFacet::error:
			std::cerr << "error";
			break;
		case CodecvtFacet::noconv:
			std::cerr << "noconv";
			break;
		default:
			std::cerr << "unknown";
		}
		std::cerr << ", please check in_cvt_state."
			<< std::endl;
	}
	std::wstring result = intern_buffer;

	delete[]intern_buffer;

	return result;
}
int main(int argc, char* argv[]) {
	cef_argc = argc;	
	if (argc > 16) {
		printf("error:argc > 16");
		return -1;
	}
	for (int i = 0; i < argc; ++i) {
		cef_argv[i] = argv[i];
	}
	auto tchmain_hmodule = dlopen("tchmain.so", RTLD_NOW);
	if (tchmain_hmodule == 0) {
		TchErrorHandler::OnTchError(-1, "Failed to load tchmain.dll.");
		return -1;
	}
	auto tchmain_ptr = (TchMain)dlsym(tchmain_hmodule, "TchMain");
	if (tchmain_ptr == 0) {
		TchErrorHandler::OnTchError(-1, "Find TchMain function failed in tchmain.");
		return -1;
	}
	int result=tchmain_ptr(argc,argv);
	dlclose(tchmain_hmodule);
	return result;

	//Run(L"https://www.baidu.com", 0,0);
	//return 0;
}
//zmg end