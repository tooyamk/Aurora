#include "GraphicsWinGL.h"
#include "utils/String.h"
#include <thread>

AE_MODULE_GRAPHICS_NS_BEGIN

GraphicsWinGL::GraphicsWinGL() :
	_isWIndowed(true),
	_tpf(0.f),
	_dc(nullptr),
	_rc(nullptr) {
}

GraphicsWinGL::~GraphicsWinGL() {
	_release();
}

void GraphicsWinGL::createView(void* style, const i8* windowTitle, const Rect<i32>& rect, bool fullscreen, f32 fps) {
	_rect.set(rect);
	_isWIndowed = fullscreen;
	setFPS(fps);

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

	HWND hWnd = CreateWindowExW(0L, wnd.lpszClassName, String::UTF8ToUnicode(windowTitle).c_str(), WS_OVERLAPPEDWINDOW,
		_rect.left, _rect.top, _rect.getWidth(), _rect.getHeight(), GetDesktopWindow(), nullptr, wnd.hInstance, nullptr);
	//HWND hWnd = CreateWindowExW(0L, wnd.lpszClassName, String::UTF8ToUnicode(windowTitle).c_str(), WS_EX_TOPMOST, x, y, w, h, nullptr, nullptr, hIns, nullptr);
	if (hWnd && _init(hWnd)) {
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);

		MSG msg;

		ZeroMemory(&msg, sizeof(msg));

		//int aa = 0;

		while (msg.message != WM_QUIT) {
			if (PeekMessage(
				&msg,     // �洢��Ϣ�Ľṹ��ָ��
				nullptr,  // ������Ϣ���߳���Ϣ���ᱻ���� 
				0,        // ��Ϣ������Сֵ; Ϊ0ʱ�������п�����Ϣ
				0,        // ��Ϣ�������ֵ; Ϊ0ʱ�������п�����Ϣ
				PM_REMOVE // ָ����Ϣ��δ���; ��Ϣ�ڴ������Ӷ������Ƴ�
			)) {
				TranslateMessage(&msg); // �任�������Ϣ���ַ���Ϣ���ַ���Ϣ�����͵������̵߳���Ϣ����
				DispatchMessage(&msg);  // �ɷ���Ϣ�����ڹ���
			} else {
				//if (++aa == 120) {
					//PostQuitMessage(0);
				//}

				DWORD timeBegin = GetTickCount();                //ѭ����ʼ��ʱ��  

				//ѡ��RC��Ϊ��ǰ�̵߳�RC
				wglMakeCurrent(_dc, _rc);

				//
				//�ڴ�ʹ��OpenGL���л���
				//

				//������ǰ�������ͺ�̨������
				SwapBuffers(_dc);

				//ȡ����ǰ�߳�ѡ�е�RC
				wglMakeCurrent(nullptr, nullptr);

				//DirectX_Update(hwnd);                          //directXѭ��
				//DirectX_Render(hwnd);                          //directX��Ⱦ
				f32 timePhase = f32(GetTickCount() - timeBegin); //ѭ���ķѵ�ʱ��
				if (timePhase < _tpf) {                           //ѭ���ķѵ�ʱ��<ÿ֡��ʱ��
					std::this_thread::sleep_for(std::chrono::milliseconds(DWORD(_tpf - timePhase))); //��ʣ���ʱ��ȴ�
				}
			}
		}
	}

	_release();
}

void GraphicsWinGL::setFPS(f32 fps) {
	_tpf = fps <= 0.f ? 0.f : 1000.f / fps;
}

bool GraphicsWinGL::isWindowed() const {
	return _isWIndowed;
}

void GraphicsWinGL::toggleFullscreen() {
}

void GraphicsWinGL::getViewRect(Rect<i32>& dst) const {
	dst.set(_rect);
}

void GraphicsWinGL::setViewRect(const Rect<i32>& rect) {
	if (!_rect.isEqual(rect)) {
	}
}

void GraphicsWinGL::shutdown() {
	PostQuitMessage(0);
}

bool GraphicsWinGL::_init(HWND hWnd) {
	_dc = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pfd;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags =
		PFD_DRAW_TO_WINDOW |            //��ʽ֧�ִ���
		PFD_SUPPORT_OPENGL |            //��ʽ����֧��OpenGL
		PFD_DOUBLEBUFFER;               //����֧��˫����
	pfd.iPixelType = PFD_TYPE_RGBA;     //����RGBA ��ʽ
	pfd.cColorBits = 24;                //ѡ��ɫ�����
	pfd.cRedBits = 0;                   //����RGBA
	pfd.cRedShift = 0;
	pfd.cGreenBits = 0;
	pfd.cGreenShift = 0;
	pfd.cBlueBits = 0;
	pfd.cBlueShift = 0;
	pfd.cAlphaBits = 0;
	pfd.cAlphaShift = 0;
	pfd.cAccumBits = 0;                 //���ۼӻ���
	pfd.cAccumRedBits = 0;              //���Ծۼ�λ
	pfd.cAccumGreenBits = 0;
	pfd.cAccumBlueBits = 0;
	pfd.cAccumAlphaBits = 0;
	pfd.cDepthBits = 32;                //32λZ-����(��Ȼ���)
	pfd.cStencilBits = 0;               //���ɰ建��
	pfd.cAuxBuffers = 0;                //�޸�������
	pfd.iLayerType = PFD_MAIN_PLANE;    //����ͼ��
	pfd.bReserved = 0;                  //Reserved
	pfd.dwLayerMask = 0;                //���Բ�����
	pfd.dwVisibleMask = 0;
	pfd.dwDamageMask = 0;

	int pf = ChoosePixelFormat(_dc, &pfd);
	SetPixelFormat(_dc, pf, &pfd);
	_rc = wglCreateContext(_dc);

	long style;
	style = GetWindowLong(hWnd, GWL_STYLE);

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

void GraphicsWinGL::_release() {
	if (_rc) {
		wglDeleteContext(_rc);
		_rc = nullptr;
	}
}

AE_MODULE_GRAPHICS_NS_END