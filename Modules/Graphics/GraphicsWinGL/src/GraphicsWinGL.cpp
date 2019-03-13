#include "GraphicsWinGL.h"
#include "utils/String.h"

AE_MODULE_GRAPHICS_NS_BEGIN

void GraphicsWinGL::createView(void* style, const i8* windowTitle, const Rect<i32>& rect, bool fullscreen, f32 fps) {
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
				if (timePhase < _tpf) {              //ѭ���ķѵ�ʱ��<ÿ֡��ʱ��
					sleepms(DWORD(_tpf - timePhase)); //��ʣ���ʱ��ȴ�
				}

				GetTickCount();
			}
		}
	}

	_release();
}

AE_MODULE_GRAPHICS_NS_END