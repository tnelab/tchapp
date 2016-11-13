#pragma once
#ifndef CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_GDI_RENDERER_H_
#define CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_GDI_RENDERER_H_
#pragma once
#include "include/cef_browser.h"
#include "include/cef_render_handler.h"
#include "include/wrapper/cef_helpers.h"

namespace Tnelab {
	class TchGDIRenderer{
	public:
		~TchGDIRenderer();
		// Initialize the OpenGL environment.
		void Initialize() {
			//MessageBox(0, L"init1", 0, 0);
			if (initialized_) return;
			DCHECK(hwnd_);			
			//执行后会默认绘制黑色背景，影响体验，需马上刷新界面
			::SetWindowLong(hwnd_, GWL_EXSTYLE, ::GetWindowLong(hwnd_, GWL_EXSTYLE) | WS_EX_LAYERED/* | WS_EX_TRANSPARENT*/);
			initialized_ = true;
		};

		void SetTargetHWND(HWND hwnd) { hwnd_ = hwnd; };

		// Clean up the GDI environment.
		void Cleanup();

		// Render to the screen.
		void Render();

		// Forwarded from CefRenderHandler callbacks.
		void OnPopupShow(CefRefPtr<CefBrowser> browser,
			bool show);
		// |rect| must be in pixel coordinates.
		void OnPopupSize(CefRefPtr<CefBrowser> browser,
			const CefRect& rect);
		void OnPaint(CefRefPtr<CefBrowser> browser,
			CefRenderHandler::PaintElementType type,
			const CefRenderHandler::RectList& dirtyRects,
			const void* buffer, int width, int height);

		// Apply spin.
		void SetSpin(float spinX, float spinY);
		void IncrementSpin(float spinDX, float spinDY);

		bool IsTransparent() const { return true; }
		cef_color_t GetBackgroundColor() const { return CefColorSetARGB(255,255,255,255); }

		int GetViewWidth() const { return view_width_; }
		int GetViewHeight() const { return view_height_; }

		const CefRect& popup_rect() const { return popup_rect_; }
		const CefRect& original_popup_rect() const { return original_popup_rect_; }

		CefRect GetPopupRectInWebView(const CefRect& original_rect);
		void ClearPopupRects();
	private:
		void createSourceHDC(HDC& hdc,HBITMAP& hbitmap,int width,int height);
		void bgra2rgba(char* out_color_bytes, char* out_transparent_color_bytes,const char* bytes_buffer,int startX,int startY,int width,int height);

		void *bmp_cnt_ = 0;
		BITMAPINFOHEADER bih_;

		HDC source_hdc_=0;
		HBITMAP source_hdc_hbitmap_=0;

		HDC transparent_source_hdc_ = 0;
		HBITMAP transparent_source_hdc_hbitmap_ = 0;

		HWND hwnd_=0;
		int view_width_=0;
		int view_height_=0;
		CefRect popup_rect_;
		CefRect original_popup_rect_;
		float spin_x_=0.0;
		float spin_y_=0.0;
		CefRect update_rect_;

		bool initialized_=false;
	};
}
#endif