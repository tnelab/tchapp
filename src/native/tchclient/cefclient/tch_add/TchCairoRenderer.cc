#include "TchCairoRenderer.h"
#include "cefclient/browser/browser_window_osr_gtk.h"
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkx.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include "include/wrapper/cef_helpers.h"

namespace {
  void slice_buffer(uint32_t* out, const uint32_t* in,
                    int startX, int startY,
                    int view_width, int view_height,
                    int buffer_width) {
    for (int h = 0; h < view_height; ++h) {
      for (int l = 0; l < view_width; ++l) {
        const uint32_t* in_pixel = in + ((h + startY) * buffer_width + l + startX);
        uint32_t* out_pixel = out + (h * view_width + l);
        *out_pixel = *in_pixel;
      }
    }
  }

  void paint_buffer(cairo_t* cr,
                    const void* buffer, std::vector<char>& sliced_buffer,
                    int startX, int startY,
                    int view_width, int view_height,
                    int buffer_width, int buffer_height) {
    // stackoverflow.com/questions/5249644/cairo-load-image-from-data
    // slice buffer if not paint the whole area
    if (startX != 0 || startY != 0 ||
        view_width != buffer_width ||
        view_height != buffer_height) {
        sliced_buffer.resize(view_width*view_height*4);
        slice_buffer(reinterpret_cast<uint32_t*>(sliced_buffer.data()),
                     reinterpret_cast<const uint32_t*>(buffer),
                     startX, startY, view_width, view_height, buffer_width);
        buffer = sliced_buffer.data();
    }
    // Clear old area
    cairo_set_source_rgba(cr, 1, 1, 1, 0);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_rectangle(cr, startX, startY, view_width, view_height);
    cairo_fill(cr);
    // printf("clear %d %d %d %d\n", startX, startY, view_width, view_height);
    // create surface from data and paint it
    int stride = cairo_format_stride_for_width(
        CAIRO_FORMAT_ARGB32, view_width);
    cairo_surface_t* surface = cairo_image_surface_create_for_data(
        reinterpret_cast<unsigned char*>(const_cast<void*>(buffer)),
        CAIRO_FORMAT_ARGB32,
        view_width, view_height, stride);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_source_surface(cr, surface, startX, startY);
    cairo_paint(cr);
    cairo_surface_flush(surface);
    cairo_surface_destroy(surface);
  }
}

namespace Tnelab {

TchCairoRenderer::TchCairoRenderer(const client::OsrRenderer::Settings& settings)
    : settings_(settings),
      initialized_(false),
      view_width_(0),
      view_height_(0),
      popup_rect_(),
      original_popup_rect_(),
      sliced_buffer_() {
}

TchCairoRenderer::~TchCairoRenderer() {
  Cleanup();
}

void TchCairoRenderer::Initialize() {
  if (initialized_)
    return;
  initialized_ = true;
}

void TchCairoRenderer::Cleanup() {
}

void TchCairoRenderer::Render() {
  if (view_width_ == 0 || view_height_ == 0)
    return;
  DCHECK(initialized_);
}

void TchCairoRenderer::OnPopupShow(CefRefPtr<CefBrowser> browser,
                              bool show) {
  if (!show) {
    // Clear the popup rectangle.
    ClearPopupRects();
  }
}

void TchCairoRenderer::OnPopupSize(CefRefPtr<CefBrowser> browser,
                              const CefRect& rect) {
  if (rect.width <= 0 || rect.height <= 0)
    return;
  original_popup_rect_ = rect;
  popup_rect_ = GetPopupRectInWebView(original_popup_rect_);
}

CefRect TchCairoRenderer::GetPopupRectInWebView(const CefRect& original_rect) {
  CefRect rc(original_rect);
  // if x or y are negative, move them to 0.
  if (rc.x < 0)
    rc.x = 0;
  if (rc.y < 0)
    rc.y = 0;
  // if popup goes outside the view, try to reposition origin
  if (rc.x + rc.width > view_width_)
    rc.x = view_width_ - rc.width;
  if (rc.y + rc.height > view_height_)
    rc.y = view_height_ - rc.height;
  // if x or y became negative, move them to 0 again.
  if (rc.x < 0)
    rc.x = 0;
  if (rc.y < 0)
    rc.y = 0;
  return rc;
}

void TchCairoRenderer::ClearPopupRects() {
  popup_rect_.Set(0, 0, 0, 0);
  original_popup_rect_.Set(0, 0, 0, 0);
}

void TchCairoRenderer::OnPaint(CefRefPtr<CefBrowser> browser,
                          client::BrowserWindowOsrGtk* browserWindow,
                          CefRenderHandler::PaintElementType type,
                          const CefRenderHandler::RectList& dirtyRects,
                          const void* buffer, int width, int height) {
  if (!initialized_)
    Initialize();

  // Create cairo object
  auto area = (GtkWidget*)browserWindow->GetWindowHandle();
  cairo_t *cr = gdk_cairo_create(area->window);

  if (type == PET_VIEW) {
    int old_width = view_width_;
    int old_height = view_height_;

    view_width_ = width;
    view_height_ = height;

    if (old_width != view_width_ || old_height != view_height_ ||
        (dirtyRects.size() == 1 &&
         dirtyRects[0] == CefRect(0, 0, view_width_, view_height_))) {
      // Update/resize the whole texture.
      // printf("render full\n");
      paint_buffer(cr, buffer, sliced_buffer_, 0, 0,
                   view_width_, view_height_, width, height);
    } else {
      // Update just the dirty rectangles.
      CefRenderHandler::RectList::const_iterator i = dirtyRects.begin();
      for (; i != dirtyRects.end(); ++i) {
        const CefRect& rect = *i;
        DCHECK(rect.x + rect.width <= view_width_);
        DCHECK(rect.y + rect.height <= view_height_);
        // printf("render dirty %d %d %d %d\n", rect.x, rect.y, rect.width, rect.height);
        paint_buffer(cr, buffer, sliced_buffer_, rect.x, rect.y,
                     rect.width, rect.height, width, height);
      }
    }
  } else if (type == PET_POPUP && popup_rect_.width > 0 &&
             popup_rect_.height > 0) {
    int x = popup_rect_.x;
    int y = popup_rect_.y;
    int w = width;
    int h = height;

    // Adjust the popup to fit inside the view.
    if (x < 0) {
      x = 0;
    }
    if (y < 0) {
      y = 0;
    }
    if (x + w > view_width_)
      w -= x + w - view_width_;
    if (y + h > view_height_)
      h -= y + h - view_height_;

    // Update the popup rectangle.
    paint_buffer(cr, buffer, sliced_buffer_, x, y, w, h, width, height);
  }
  // Destroy cairo object
  cairo_destroy(cr);
}

}  // namespace client
