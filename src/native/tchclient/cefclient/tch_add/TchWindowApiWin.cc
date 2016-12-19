#include "TchWindowApi.h"
#include "include/cef_parser.h"
#include "include/wrapper/cef_message_router.h"
#include "cefclient/tch_add/TchWindowApi.h"
#include "cefclient/browser/root_window.h"
#include <windows.h>
using namespace Tnelab;

namespace {
	scoped_refptr<client::RootWindow> GetRootWindow(CefRefPtr<CefBrowser> browser) {
		scoped_refptr<client::RootWindow> root_window =
			client::RootWindow::GetForBrowser(browser->GetIdentifier());
		return root_window;
	}
}

void TchWindowApi::CloseWindow(CefRefPtr<CefFrame> frame) {

	HWND root_window_handle = GetRootWindow(frame->GetBrowser())->GetWindowHandle();
	//::CloseWindow(root_window_handle);
	::SendMessage(root_window_handle, WM_CLOSE, 0, 0);
}
void TchWindowApi::MinimizeWindow(CefRefPtr<CefFrame> frame) {
	HWND root_window_handle = GetRootWindow(frame->GetBrowser())->GetWindowHandle();
	::ShowWindow(root_window_handle, SW_SHOWMINIMIZED);
}
void TchWindowApi::MaximizingWindow(CefRefPtr<CefFrame> frame) {
	HWND root_window_handle = GetRootWindow(frame->GetBrowser())->GetWindowHandle();
	SetWindowLong(root_window_handle, GWL_STYLE, GetWindowLong(root_window_handle, GWL_STYLE) &~WS_POPUP| WS_CAPTION);
	::ShowWindow(root_window_handle, SW_SHOWMAXIMIZED);
}
void TchWindowApi::RestoreWindow(CefRefPtr<CefFrame> frame) {
	HWND root_window_handle = GetRootWindow(frame->GetBrowser())->GetWindowHandle();
	SetWindowLong(root_window_handle, GWL_STYLE, GetWindowLong(root_window_handle, GWL_STYLE) & ~WS_CAPTION |WS_POPUP);
	::ShowWindow(root_window_handle, SW_SHOWNORMAL);
}
void TchWindowApi::HideWindow(CefRefPtr<CefFrame> frame) {
	HWND root_window_handle = GetRootWindow(frame->GetBrowser())->GetWindowHandle();
	::ShowWindow(root_window_handle, SW_HIDE);
}
void TchWindowApi::ShowWindow(CefRefPtr<CefFrame> frame) {
	HWND root_window_handle = GetRootWindow(frame->GetBrowser())->GetWindowHandle();
	::ShowWindow(root_window_handle, SW_SHOW);
}
void TchWindowApi::SetWindowPos(CefRefPtr<CefFrame> frame, int x, int y, int width, int height) {
	HWND root_window_handle = GetRootWindow(frame->GetBrowser())->GetWindowHandle();
	::SetWindowPos(root_window_handle, 0, x, y, width, height, SWP_NOZORDER);
}
void TchWindowApi::GetWindowPos(CefRefPtr<CefFrame> frame,CefRect& rect) {
	HWND root_window_handle = GetRootWindow(frame->GetBrowser())->GetWindowHandle();
	RECT w_rect;
	::GetWindowRect(root_window_handle, &w_rect);
	rect.x = w_rect.left;
	rect.y = w_rect.top;
	rect.width = w_rect.right - w_rect.left;
	rect.height = w_rect.bottom - w_rect.top;
}