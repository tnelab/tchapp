#include "TchGDIRenderer.h"

Tnelab::TchGDIRenderer::~TchGDIRenderer()
{
	Cleanup();
}

void Tnelab::TchGDIRenderer::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect & rect)
{
	if (rect.width <= 0 || rect.height <= 0)
		return;
	original_popup_rect_ = rect;
	popup_rect_ = GetPopupRectInWebView(original_popup_rect_);
}

void Tnelab::TchGDIRenderer::OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList & dirtyRects, const void * buffer, int width, int height)
{
	if (this->hwnd_ == 0) return;
	if (width != view_width_ || height != view_height_) {
		this->createSourceHDC(this->source_hdc_, this->source_hdc_hbitmap_, width, height);
		this->createSourceHDC(this->transparent_source_hdc_, this->transparent_source_hdc_hbitmap_, width, height);
	}
	int old_width = view_width_;
	int old_height = view_height_;

	view_width_ = width;
	view_height_ = height;
	if (width == 0 || height == 0) return;
	
	//准备web渲染DC
	char* bytes_buffer = (char*)buffer;
	if (type == PET_VIEW) {
		//update_rect_ = dirtyRects[0];		

		if (old_width != view_width_ || old_height != view_height_ ||
			(dirtyRects.size() == 1 &&
				dirtyRects[0] == CefRect(0, 0, view_width_, view_height_))) {
			// Update/resize.			
			//bgra到rgba转换
			char* color_bytes = (char*)malloc(view_height_*view_width_*4);
			char* transparent_color_bytes = (char*)malloc(view_height_*view_width_ * 4);
			bgra2rgba(color_bytes, transparent_color_bytes, bytes_buffer, 0, 0, view_width_, view_height_);
			//输出到source_hdc
			SetDIBits(source_hdc_, source_hdc_hbitmap_, 0, view_height_, color_bytes, (BITMAPINFO*)&bih_, DIB_RGB_COLORS);
			//准备全透明遮罩DC
			SetDIBits(transparent_source_hdc_, transparent_source_hdc_hbitmap_, 0, view_height_, transparent_color_bytes, (BITMAPINFO*)&bih_, DIB_RGB_COLORS);
			delete(color_bytes);
			delete(transparent_color_bytes);
		}
		else {			
			// Update just the dirty rectangles.
			CefRenderHandler::RectList::const_iterator i = dirtyRects.begin();
			for (; i != dirtyRects.end(); ++i) {
				const CefRect& rect = *i;
				DCHECK(rect.x + rect.width <= view_width_);
				DCHECK(rect.y + rect.height <= view_height_);
				char* color_bytes = (char*)malloc(rect.width*rect.height*4);
				char* transparent_color_bytes = (char*)malloc(rect.width*rect.height * 4);
				bgra2rgba(color_bytes, bytes_buffer, transparent_color_bytes,rect.x, rect.y, rect.width, rect.height);
				//局部刷新source_hdec				
				StretchDIBits(source_hdc_,rect.x, rect.y, rect.width,rect.height, rect.x, rect.y, rect.width, rect.height, color_bytes, (BITMAPINFO*)&bih_, DIB_RGB_COLORS,0);
				//准备全透明遮罩DC
				StretchDIBits(transparent_source_hdc_, rect.x, rect.y, rect.width, rect.height, rect.x, rect.y, rect.width, rect.height, transparent_color_bytes, (BITMAPINFO*)&bih_, DIB_RGB_COLORS, 0);
				delete(color_bytes);
				delete(transparent_color_bytes);
			}
		}
	}
	else if (type == PET_POPUP && popup_rect_.width > 0 &&
		popup_rect_.height > 0) {
		int skip_pixels = 0, x = popup_rect_.x;
		int skip_rows = 0, y = popup_rect_.y;
		int w = width;
		int h = height;

		// Adjust the popup to fit inside the view.
		if (x < 0) {
			skip_pixels = -x;
			x = 0;
		}
		if (y < 0) {
			skip_rows = -y;
			y = 0;
		}
		if (x + w > view_width_)
			w -= x + w - view_width_;
		if (y + h > view_height_)
			h -= y + h - view_height_;

		// Update the popup rectangle.
		char* color_bytes = (char*)malloc(w*h * 4);
		char* transparent_color_bytes = (char*)malloc(w*h * 4);
		bgra2rgba(color_bytes, transparent_color_bytes,bytes_buffer, x, y, w, h);
		//局部刷新source_hdec				
		StretchDIBits(source_hdc_, x, y, w, h, x, y, w, h, color_bytes, (BITMAPINFO*)&bih_, DIB_RGB_COLORS, 0);
		//准备全透明遮罩DC
		StretchDIBits(transparent_source_hdc_, x, y, w, h, x, y, w, h, transparent_color_bytes, (BITMAPINFO*)&bih_, DIB_RGB_COLORS, 0);
		delete(color_bytes);
		delete(transparent_color_bytes);
	}
}

void Tnelab::TchGDIRenderer::SetSpin(float spinX, float spinY)
{
	spin_x_ = spinX;
	spin_y_ = spinY;
}

void Tnelab::TchGDIRenderer::IncrementSpin(float spinDX, float spinDY)
{
	spin_x_ -= spinDX;
	spin_y_ -= spinDY;
}

void Tnelab::TchGDIRenderer::Render()
{	
	if (view_width_ == 0 || view_height_ == 0)
		return;
	if (hwnd_ == 0)
		return;
	//BitBlt(GetDC(hwnd_), 0, 0, view_width_, view_height_, source_hdc_, 0, 0, SRCCOPY);
	
	BLENDFUNCTION bfunc = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	SIZE size;
	POINT point;
	point.x = 0;
	point.y = 0;
	size.cx = view_width_;
	size.cy = view_height_;
	if (!initialized_) this->Initialize();//消除启动黑屏
	//呈现到父窗口
	bool result=UpdateLayeredWindow(GetParent(hwnd_), GetDC(GetParent(hwnd_)), NULL, &size, source_hdc_, &point, 0, &bfunc, ULW_ALPHA);
	DCHECK(result);
	//浏览器窗口设置为1透明遮罩
	result = UpdateLayeredWindow(hwnd_, GetDC(hwnd_), NULL, &size, transparent_source_hdc_, &point, 0, &bfunc, ULW_ALPHA);
	DCHECK(result);
}

void Tnelab::TchGDIRenderer::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{
	if (!show) {
		// Clear the popup rectangle.
		ClearPopupRects();
	}
}

void Tnelab::TchGDIRenderer::Cleanup()
{
	if (source_hdc_ != 0) {

		RestoreDC(source_hdc_, -1);
		DeleteDC(source_hdc_);
		DeleteObject(this->source_hdc_hbitmap_);

		RestoreDC(transparent_source_hdc_,-1);
		DeleteDC(transparent_source_hdc_);
		DeleteObject(this->transparent_source_hdc_hbitmap_);
	}
}

CefRect Tnelab::TchGDIRenderer::GetPopupRectInWebView(const CefRect & original_rect)
{
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

void Tnelab::TchGDIRenderer::ClearPopupRects()
{
	popup_rect_.Set(0, 0, 0, 0);
	original_popup_rect_.Set(0, 0, 0, 0);
}

void Tnelab::TchGDIRenderer::createSourceHDC(HDC& hdc, HBITMAP& hbitmap, int width, int height)
{
	if (hdc != 0) this->Cleanup();
	DCHECK(width > 0);
	DCHECK(height > 0);

	int iSize = sizeof(BITMAPINFOHEADER);	
	memset(&bih_, 0, iSize);
	bih_.biSize = iSize;
	bih_.biWidth = width;
	bih_.biHeight = height;
	bih_.biPlanes = 1;
	bih_.biBitCount = 32;
	bih_.biCompression = BI_RGB;
	
	hdc = CreateCompatibleDC(NULL);	
	DCHECK(hdc);
	SaveDC(hdc);
	hbitmap = CreateDIBSection(
		hdc,
		(BITMAPINFO*)&bih_,
		DIB_RGB_COLORS,
		&bmp_cnt_,
		NULL,
		0);
	DCHECK(hbitmap);
	DCHECK(bmp_cnt_);
	if (hbitmap)
		SelectObject(hdc, hbitmap);

	PIXELFORMATDESCRIPTOR pfd;
	int format;	
	
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_BITMAP|PFD_SUPPORT_GDI;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	format = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, format, &pfd);
}

void Tnelab::TchGDIRenderer::bgra2rgba(char * out_color_bytes, char* out_transparent_color_bytes, const char * bytes_buffer, int startX, int startY, int width, int height)
{
	ZeroMemory(out_transparent_color_bytes, width*height * 4);
	for (int h =0; h < height; ++h) {
		for (int l = 0; l < width;++l) {
			out_color_bytes[(h*width + l) * 4 + 0] = bytes_buffer[((height - 1 - (startY + h )) * width + startX + l) * 4 + 0];//red
			out_color_bytes[(h*width + l) * 4 + 1] = bytes_buffer[((height - 1 - (startY + h )) * width + startX + l) * 4 + 1];//green
			out_color_bytes[(h*width + l) * 4 + 2] = bytes_buffer[((height - 1 - (startY + h )) * width + startX + l) * 4 + 2];//blue
			out_color_bytes[(h*width + l) * 4 + 3] = bytes_buffer[((height - 1 - (startY + h )) * width + startX + l) * 4 + 3];//alpha

			out_transparent_color_bytes[(h*width + l) * 4 + 3] = 1;
		}
	}
}

