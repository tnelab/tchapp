#ifndef CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_WINDOW_API_H_
#define CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_WINDOW_API_H_
#pragma once
#include "include/cef_frame.h"

namespace Tnelab {
	class TchWindowApi {
	public:
		static CefRect CaptionRect;
		static void CloseWindow(CefRefPtr<CefFrame> frame);
		static void MinimizeWindow(CefRefPtr<CefFrame> frame);
		static void MaximizingWindow(CefRefPtr<CefFrame> frame);
		static void RestoreWindow(CefRefPtr<CefFrame> frame);
		static void HideWindow(CefRefPtr<CefFrame> frame);
		static void ShowWindow(CefRefPtr<CefFrame> frame);
	};
}
#endif