#include "GraphicsWinD3D9.h"
#include "utils/String.h"

AE_MODULE_GRAPHICS_NS_BEGIN

GraphicsWinD3D9::GraphicsWinD3D9() :
	_hIns(nullptr),
	_d3d(nullptr),
	_d3dDevice(nullptr),
	_tpf(0.f) {
	ZeroMemory(&_d3dpp, sizeof(_d3dpp));
	_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // ֡������������ʽ; ������COPY������FLIP�����豸��ȷ���ʺϵ�ǰ����ķ�ʽ
}

GraphicsWinD3D9::~GraphicsWinD3D9() {
	_release();
}

void GraphicsWinD3D9::createView(void* style, const i8* windowTitle, const Rect<i32>& rect, bool fullscreen, f32 fps) {
	_rect.set(rect);
	_d3dpp.Windowed = !fullscreen;
	_d3dpp.FullScreen_RefreshRateInHz = fullscreen ? D3DPRESENT_RATE_DEFAULT : 0;
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
	if (wnd.hInstance) {
		_hIns = wnd.hInstance;
	} else {
		_hIns = GetModuleHandle(nullptr);
		wnd.hInstance = _hIns;
	}
	_className = wnd.lpszClassName;

	RegisterClassExW(&wnd);

	HWND hWnd = CreateWindowExW(0L, wnd.lpszClassName, String::UTF8ToUnicode(windowTitle).c_str(), WS_OVERLAPPEDWINDOW, 
		_rect.left, _rect.top, _rect.getWidth(), _rect.getHeight(), GetDesktopWindow(), nullptr, _hIns, nullptr);
	//HWND hWnd = CreateWindowExW(0L, wnd.lpszClassName, String::UTF8ToUnicode(windowTitle).c_str(), WS_EX_TOPMOST, x, y, w, h, nullptr, nullptr, hIns, nullptr);
	if (hWnd && _init(hWnd)) {
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);

		MSG msg;

		ZeroMemory(&msg, sizeof(msg));

		//int aa = 0;

		while (msg.message != WM_QUIT) {
			if (PeekMessage(
				&msg, // �洢��Ϣ�Ľṹ��ָ��
				nullptr, // ������Ϣ���߳���Ϣ���ᱻ���� 
				0, // ��Ϣ������Сֵ; Ϊ0ʱ�������п�����Ϣ
				0, // ��Ϣ�������ֵ; Ϊ0ʱ�������п�����Ϣ
				PM_REMOVE // ָ����Ϣ��δ���; ��Ϣ�ڴ������Ӷ������Ƴ�
			)) {
				TranslateMessage(&msg); // �任�������Ϣ���ַ���Ϣ���ַ���Ϣ�����͵������̵߳���Ϣ����
				DispatchMessage(&msg); // �ɷ���Ϣ�����ڹ���
			} else {
				//if (++aa == 120) {
					//PostQuitMessage(0);
				//}

				DWORD timeBegin = GetTickCount();             //ѭ����ʼ��ʱ��  

				_d3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(_d3dpp.Windowed ? 0xFF : 0, 0, 0), 1.0f, 0);
				_d3dDevice->BeginScene();
				// 3Dͼ������
				_d3dDevice->EndScene();

				// ��ʾbackbuffer���ݵ���Ļ
				_d3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

				//DirectX_Update(hwnd);                         //directXѭ��
				//DirectX_Render(hwnd);                         //directX��Ⱦ
				f32 timePhase = f32(GetTickCount() - timeBegin); //ѭ���ķѵ�ʱ��
				if (timePhase < _tpf){              //ѭ���ķѵ�ʱ��<ÿ֡��ʱ��
					sleepms(DWORD(_tpf - timePhase)); //��ʣ���ʱ��ȴ�
				}

				GetTickCount();
			}
		}
	}

	_release();
}

void GraphicsWinD3D9::setFPS(f32 fps) {
	_tpf = fps <= 0.f ? 0.f : 1000.f / fps;
}

bool GraphicsWinD3D9::isWindowed() const {
	return _d3dpp.Windowed;
}

void GraphicsWinD3D9::toggleFullscreen() {
	//OnDeviceLost();

	_toggleFullscreen();
	
	HRESULT rst = _d3dDevice->Reset(&_d3dpp);
	if (!SUCCEEDED(rst)) {
		_toggleFullscreen();
		_d3dDevice->Reset(&_d3dpp);
	}

	if (_d3dpp.Windowed) {
		SetWindowPos(_d3dpp.hDeviceWindow, HWND_TOP, _rect.left, _rect.top, _rect.getWidth(), _rect.getHeight(), SWP_NOZORDER | SWP_DRAWFRAME | SWP_SHOWWINDOW);
	}

	//OnDeviceRestore();
}

void GraphicsWinD3D9::getViewRect(Rect<i32>& dst) const {
	dst.set(_rect);
}

void GraphicsWinD3D9::setViewRect(const Rect<i32>& rect) {
	if (!_rect.isEqual(rect)) {
		if (_d3dpp.Windowed) {
			if (_rect.getWidth() == rect.getWidth() && _rect.getWidth() == rect.getHeight()) {
				_rect.set(rect);
				SetWindowPos(_d3dpp.hDeviceWindow, HWND_TOP, _rect.left, _rect.top, _rect.getWidth(), _rect.getHeight(), SWP_NOZORDER | SWP_DRAWFRAME | SWP_SHOWWINDOW);
			} else {
				//OnDeviceLost();

				HRESULT rst = _d3dDevice->Reset(&_d3dpp);
				if (SUCCEEDED(rst)) {
					_rect.set(rect);
					SetWindowPos(_d3dpp.hDeviceWindow, HWND_TOP, _rect.left, _rect.top, _rect.getWidth(), _rect.getHeight(), SWP_NOZORDER | SWP_DRAWFRAME | SWP_SHOWWINDOW);
				}

				//OnDeviceRestore();
			}
		} else {
			_rect.set(rect);
		}
	}
}

void GraphicsWinD3D9::shutdown() {
	PostQuitMessage(0);
}

void GraphicsWinD3D9::_toggleFullscreen() {
	_d3dpp.Windowed = !_d3dpp.Windowed;
	if (_d3dpp.Windowed) {
		_d3dpp.FullScreen_RefreshRateInHz = 0;
	} else {
		RECT rect;
		GetWindowRect(_d3dpp.hDeviceWindow, &rect);
		_rect.set(rect.left, rect.top, rect.right, rect.bottom);

		_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		_d3dpp.BackBufferWidth = GetSystemMetrics(SM_CXSCREEN);
		_d3dpp.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);
	}
}

bool GraphicsWinD3D9::_init(HWND hWnd) {
	// ��ʾģʽ ��������Ϊ��λ����Ļ��ߣ�ˢ��Ƶ�ʣ�surface formt��
	D3DDISPLAYMODE displayMode;

	// ����D3D���� ��ȡ���Կ�Ӳ����Ϣ  ���ȱ�����������ͷ�
	_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!_d3d) return false;

	// ��ȡ��ʾģʽ
	if (FAILED(_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT/*��ѯ���Կ�*/, &displayMode))) return false;

	_d3dpp.hDeviceWindow = hWnd;
	_d3dpp.BackBufferFormat = displayMode.Format;  // D3DFMT_X8R8G8B8  ��ʾΪ32λRGB���ظ�ʽ ÿ����ɫ��һ���ֽڱ�ʾ
	// ����D3D�豸���� ---- �����Կ�
	if (FAILED(_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &_d3dpp, &_d3dDevice))) return false;

	return true;
}

void GraphicsWinD3D9::_release() {
	if (_d3dDevice) {
		_d3dDevice->Release();
		_d3dDevice = nullptr;
	}
	if (_d3d) {
		_d3d->Release();
		_d3d = nullptr;
	}
	if (_hIns) {
		UnregisterClassW(_className.c_str(), _hIns);
		_hIns = nullptr;
	}
}

AE_MODULE_GRAPHICS_NS_END