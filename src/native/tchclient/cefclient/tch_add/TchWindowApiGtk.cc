#include "TchWindowApi.h"
#include "include/wrapper/cef_helpers.h"
#include "include/cef_parser.h"
#include "include/wrapper/cef_message_router.h"
#include "cefclient/browser/root_window.h"
using namespace Tnelab;

namespace {
	GtkWindow* GetWindow(CefRefPtr<CefBrowser> browser) {
		scoped_refptr<client::RootWindow> root_window =
			client::RootWindow::GetForBrowser(browser->GetIdentifier());
		if (root_window.get())
			return GTK_WINDOW(root_window->GetWindowHandle());
		return NULL;
	}
}

void TchWindowApi::CloseWindow(CefRefPtr<CefFrame> frame) {
	frame->GetBrowser()->GetHost()->CloseBrowser(false);
}

void TchWindowApi::MinimizeWindow(CefRefPtr<CefFrame> frame) {
	auto window = GetWindow(frame->GetBrowser());
	if (window != nullptr) {
		gtk_window_iconify(window);
	}
}

void TchWindowApi::MaximizingWindow(CefRefPtr<CefFrame> frame) {
	auto window = GetWindow(frame->GetBrowser());
	if (window != nullptr) {
		gtk_window_maximize(window);
	}
}

void TchWindowApi::RestoreWindow(CefRefPtr<CefFrame> frame) {
	auto window = GetWindow(frame->GetBrowser());
	if (window != nullptr) {
		gtk_window_unmaximize(window);
	}
}

void TchWindowApi::HideWindow(CefRefPtr<CefFrame> frame) {
	auto window = GetWindow(frame->GetBrowser());
	if (window != nullptr) {
		gtk_widget_hide(GTK_WIDGET(window));
	}
}

void TchWindowApi::ShowWindow(CefRefPtr<CefFrame> frame) {
	auto window = GetWindow(frame->GetBrowser());
	if (window != nullptr) {
		gtk_widget_show_all(GTK_WIDGET(window));
		gtk_window_present(window);
	}
}

void TchWindowApi::SetWindowPos(CefRefPtr<CefFrame> frame, int x, int y, int width, int height) {
	auto window = GetWindow(frame->GetBrowser());
	if (window != nullptr) {
		gtk_window_move(GTK_WINDOW(window), x, y);
		gtk_window_resize(GTK_WINDOW(window), width, height);
	}
}

void TchWindowApi::GetWindowPos(CefRefPtr<CefFrame> frame, CefRect& rect) {
	auto window = GetWindow(frame->GetBrowser());
	if (window != nullptr) {
		gint x = 0, y = 0, width = 0, height = 0;
		gtk_window_get_position(GTK_WINDOW(window), &x, &y);
		gtk_window_get_size(GTK_WINDOW(window), &width, &height);
		rect.x = x;
		rect.y = y;
		rect.width = width;
		rect.height = height;
	}
}
