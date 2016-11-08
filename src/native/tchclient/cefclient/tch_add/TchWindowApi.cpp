#include "TchWindowApi.h"
#include "include/wrapper/cef_helpers.h"
#include "include/cef_parser.h"
#include "include/wrapper/cef_message_router.h"
#include <windows.h>
using namespace Tnelab;

CefRect TchWindowApi::CaptionRect;

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
	HWND root_window_handle = ::GetParent(frame->GetBrowser()->GetHost()->GetWindowHandle());
	RECT rt;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);   // 获得工作区大小
	::ShowWindow(root_window_handle, SW_SHOWMAXIMIZED);
	::SetWindowPos(root_window_handle, HWND_TOPMOST,rt.left, rt.top, rt.right, rt.bottom, 0);	
}
void TchWindowApi::RestoreWindow(CefRefPtr<CefFrame> frame) {
	HWND root_window_handle = ::GetParent(frame->GetBrowser()->GetHost()->GetWindowHandle());
	::ShowWindow(root_window_handle, SW_RESTORE);
}
void TchWindowApi::HideWindow(CefRefPtr<CefFrame> frame) {
	HWND root_window_handle = ::GetParent(frame->GetBrowser()->GetHost()->GetWindowHandle());
	::ShowWindow(root_window_handle, SW_HIDE);
}
void TchWindowApi::ShowWindow(CefRefPtr<CefFrame> frame) {
	HWND root_window_handle = ::GetParent(frame->GetBrowser()->GetHost()->GetWindowHandle());
	::ShowWindow(root_window_handle, SW_SHOW);
}