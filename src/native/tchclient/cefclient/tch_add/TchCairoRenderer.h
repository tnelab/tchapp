#ifndef CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_CAIRO_RENDERER_H_
#define CEF_TESTS_CEFCLIENT_TCH_ADD_TCH_CAIRO_RENDERER_H_
#pragma once

#include "include/cef_browser.h"
#include "include/cef_render_handler.h"
#include "cefclient/browser/osr_renderer.h"

namespace client {
	class BrowserWindowOsrGtk;
}

namespace Tnelab {

class TchCairoRenderer {
 public:
  explicit TchCairoRenderer(const client::OsrRenderer::Settings& settings);
  ~TchCairoRenderer();

  // Initialize the cairo environment.
  void Initialize();

  // Clean up the cairo environment.
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
               client::BrowserWindowOsrGtk* browserWindow,
               CefRenderHandler::PaintElementType type,
               const CefRenderHandler::RectList& dirtyRects,
               const void* buffer, int width, int height);

  bool IsTransparent() const { return settings_.transparent; }
  cef_color_t GetBackgroundColor() const { return settings_.background_color; }

  int GetViewWidth() const { return view_width_; }
  int GetViewHeight() const { return view_height_; }

  const CefRect& popup_rect() const { return popup_rect_; }
  const CefRect& original_popup_rect() const { return original_popup_rect_; }

  CefRect GetPopupRectInWebView(const CefRect& original_rect);
  void ClearPopupRects();

 private:
  const client::OsrRenderer::Settings settings_;
  bool initialized_;
  int view_width_;
  int view_height_;
  CefRect popup_rect_;
  CefRect original_popup_rect_;
  std::vector<char> sliced_buffer_;

  DISALLOW_COPY_AND_ASSIGN(TchCairoRenderer);
};

}  // namespace client

#endif  // CEF_TESTS_CEFCLIENT_BROWSER_OSR_RENDERER_H_

