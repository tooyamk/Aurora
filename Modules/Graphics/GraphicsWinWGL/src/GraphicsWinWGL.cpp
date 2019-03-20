#include "GraphicsWinWGL.h"
#include "utils/String.h"
#include <thread>

namespace aurora::module::graphics{
	GraphicsWinWGL::GraphicsWinWGL() :
		_isWIndowed(true),
		_dc(nullptr),
		_rc(nullptr) {
	}

	GraphicsWinWGL::~GraphicsWinWGL() {
		_release();
	}

	bool GraphicsWinWGL::createView(void* style, const i8* windowTitle, const Rect<i32>& rect, bool fullscreen) {
		_rect.set(rect);
		_isWIndowed = fullscreen;

		WNDCLASSEXW wnd = *(WNDCLASSEXW*)style;
		if (!wnd.cbSize) wnd.cbSize = sizeof(WNDCLASSEXW);
		if (!wnd.lpfnWndProc) wnd.lpfnWndProc = [](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			switch (msg) {
			case WM_DESTROY:
				PostQuitMessage(0);
				return (LRESULT)0;
				break;
			default:
				break;
			}

			return DefWindowProc(hWnd, msg, wParam, lParam);
		};
		if (!wnd.hInstance) wnd.hInstance = GetModuleHandle(nullptr);

		_className = wnd.lpszClassName;

		RegisterClassExW(&wnd);
		_hWnd = CreateWindowExW(0L, wnd.lpszClassName, String::Utf8ToUnicode(windowTitle).c_str(), WS_OVERLAPPEDWINDOW,
			_rect.left, _rect.top, _rect.getWidth(), _rect.getHeight(), GetDesktopWindow(), nullptr, wnd.hInstance, nullptr);
		//HWND hWnd = CreateWindowExW(0L, wnd.lpszClassName, String::UTF8ToUnicode(windowTitle).c_str(), WS_EX_TOPMOST, x, y, w, h, nullptr, nullptr, hIns, nullptr);
		if (_init(_hWnd)) {
			ShowWindow(_hWnd, SW_SHOWDEFAULT);
			UpdateWindow(_hWnd);

			return true;
		}

		return false;
	}

	bool GraphicsWinWGL::isWindowed() const {
		return _isWIndowed;
	}

	void GraphicsWinWGL::toggleFullscreen() {
	}

	void GraphicsWinWGL::getViewRect(Rect<i32>& dst) const {
		dst.set(_rect);
	}

	void GraphicsWinWGL::setViewRect(const Rect<i32>& rect) {
		if (!_rect.isEqual(rect)) {
		}
	}

	void GraphicsWinWGL::beginRender() {
		wglMakeCurrent(_dc, _rc);
	}

	void GraphicsWinWGL::endRender() {
		//交换当前缓冲区和后台缓冲区
		SwapBuffers(_dc);

		//取消当前线程选中的RC
		wglMakeCurrent(nullptr, nullptr);
	}

	void GraphicsWinWGL::present() {
	}

	void GraphicsWinWGL::clear() {
		
	}

	bool GraphicsWinWGL::_init(HWND hWnd) {
		_dc = GetDC(hWnd);

		PIXELFORMATDESCRIPTOR pfd;
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags =
			PFD_DRAW_TO_WINDOW |            //格式支持窗口
			PFD_SUPPORT_OPENGL |            //格式必须支持OpenGL
			PFD_DOUBLEBUFFER;               //必须支持双缓冲
		pfd.iPixelType = PFD_TYPE_RGBA;     //申请RGBA 格式
		pfd.cColorBits = 24;                //选定色彩深度
		pfd.cRedBits = 0;                   //忽略RGBA
		pfd.cRedShift = 0;
		pfd.cGreenBits = 0;
		pfd.cGreenShift = 0;
		pfd.cBlueBits = 0;
		pfd.cBlueShift = 0;
		pfd.cAlphaBits = 0;
		pfd.cAlphaShift = 0;
		pfd.cAccumBits = 0;                 //无累加缓存
		pfd.cAccumRedBits = 0;              //忽略聚集位
		pfd.cAccumGreenBits = 0;
		pfd.cAccumBlueBits = 0;
		pfd.cAccumAlphaBits = 0;
		pfd.cDepthBits = 32;                //32位Z-缓存(深度缓存)
		pfd.cStencilBits = 0;               //无蒙板缓存
		pfd.cAuxBuffers = 0;                //无辅助缓存
		pfd.iLayerType = PFD_MAIN_PLANE;    //主绘图层
		pfd.bReserved = 0;                  //Reserved
		pfd.dwLayerMask = 0;                //忽略层遮罩
		pfd.dwVisibleMask = 0;
		pfd.dwDamageMask = 0;

		int pf = ChoosePixelFormat(_dc, &pfd);
		SetPixelFormat(_dc, pf, &pfd);
		_rc = wglCreateContext(_dc);

		long style = GetWindowLong(hWnd, GWL_STYLE);

		DEVMODE dmScreenSettings;	 // Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);	 // Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = GetSystemMetrics(SM_CXSCREEN);	 // Selected Screen Width
		dmScreenSettings.dmPelsHeight = GetSystemMetrics(SM_CYSCREEN);	 // Selected Screen Height
		dmScreenSettings.dmBitsPerPel = 32;	 // Selected Bits Per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		SetWindowLong(hWnd, GWL_STYLE, style&(~WS_OVERLAPPEDWINDOW));
		SetWindowPos(hWnd,
			HWND_TOPMOST, 0, 0,
			GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN),
			SWP_SHOWWINDOW);

		return true;
	}

	void GraphicsWinWGL::_release() {
		if (_rc) {
			wglDeleteContext(_rc);
			_rc = nullptr;
		}
	}
}