#include "TchWindowApi.h"
#include "include/cef_parser.h"
#include "include/wrapper/cef_message_router.h"
#include "cefclient/tch_add/TchWindowApi.h"
#include <windows.h>
using namespace Tnelab;

void TchWindowApi::CloseWindow(CefRefPtr<CefFrame> frame) {

	HWND root_window_handle =::GetParent(frame->GetBrowser()->GetHost()->GetWindowHandle());
	//::CloseWindow(root_window_handle);
	::SendMessage(root_window_handle, WM_CLOSE, 0, 0);
}
void TchWindowApi::MinimizeWindow(CefRefPtr<CefFrame> frame) {
	HWND root_window_handle = ::GetParent(frame->GetBrowser()->GetHost()->GetWindowHandle());
	::ShowWindow(root_window_handle, SW_SHOWMINIMIZED);
}
void TchWindowApi::MaximizingWindow(CefRefPtr<CefFrame> frame) {
	HWND browser_window_handle = frame->GetBrowser()->GetHost()->GetWindowHandle();
	auto settings_id = reinterpret_cast<unsigned long>(browser_window_handle);
	auto settings = TchWindowApi::GetSettings(settings_id);
	HWND root_window_handle = ::GetParent(browser_window_handle);
	RECT rect;
	GetWindowRect(root_window_handle, &rect);
	CefRect cef_rect(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
	settings->OldRootWindowRect = cef_rect;
	RECT rt;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);   // 获得工作区大小
	::SetWindowPos(root_window_handle, NULL,rt.left, rt.top, rt.right, rt.bottom, SWP_NOZORDER);
}
void TchWindowApi::RestoreWindow(CefRefPtr<CefFrame> frame) {
	HWND browser_window_handle = frame->GetBrowser()->GetHost()->GetWindowHandle();
	auto settings_id = reinterpret_cast<unsigned long>(browser_window_handle);
	auto settings = TchWindowApi::GetSettings(settings_id);
	HWND root_window_handle = ::GetParent(browser_window_handle);
	::SetWindowPos(root_window_handle, NULL, settings->OldRootWindowRect.x, settings->OldRootWindowRect.y, settings->OldRootWindowRect.width, settings->OldRootWindowRect.height, SWP_NOZORDER);
}
void TchWindowApi::HideWindow(CefRefPtr<CefFrame> frame) {
	HWND root_window_handle = ::GetParent(frame->GetBrowser()->GetHost()->GetWindowHandle());
	::ShowWindow(root_window_handle, SW_HIDE);
}
void TchWindowApi::ShowWindow(CefRefPtr<CefFrame> frame) {
	HWND root_window_handle = ::GetParent(frame->GetBrowser()->GetHost()->GetWindowHandle());
	::ShowWindow(root_window_handle, SW_SHOW);
}
