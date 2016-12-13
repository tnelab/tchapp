#ifndef CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_WINDOW_API_H_
#define CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_WINDOW_API_H_
#pragma once
#include "include/wrapper/cef_helpers.h"
#include "include/cef_frame.h"
#include <vector>
#include "../../../tch_include/TchTypedef.h"

namespace Tnelab {	
	class TchWindowApi {
	public:
		class TchWindowSettings {
		public:
			CefRect CaptionRect;
			int WindowBorderWidth = 0;
		};
		static TchAppStartSettings StartSettings;
		static void CloseWindow(CefRefPtr<CefFrame> frame);
		static void MinimizeWindow(CefRefPtr<CefFrame> frame);
		static void MaximizingWindow(CefRefPtr<CefFrame> frame);
		static void RestoreWindow(CefRefPtr<CefFrame> frame);
		static void HideWindow(CefRefPtr<CefFrame> frame);
		static void ShowWindow(CefRefPtr<CefFrame> frame);
		static void ClearSettings(unsigned long settings_id);
		static TchWindowSettings* GetSettings(unsigned long settings_id);
		static void SetSettings(unsigned long settings_id, TchWindowSettings* ptr_settings);
	private:
		static std::map<unsigned long, TchWindowSettings*> map_settings_;
	};	
}
#endif