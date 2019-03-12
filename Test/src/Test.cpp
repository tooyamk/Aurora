// Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//#define WIN32_LEAN_AND_MEAN

//#include <windows.h>
//#include <tchar.h>
//#include <string>
//#include "shellapi.h"

//#include <iostream>

#include <d3d9.h>//must
#pragma comment(lib, "d3d9.lib")//must


#include "base/Aurora.h"
#include "base/Ref.h"
#include "nodes/Node.h"
#include "math/Matrix44.h"
#include "base/Console.h"
#include "math/Math.h"
#include "modules/GraphicsModule.h"

USING_AE_NS

PDIRECT3D9 g_D3D = nullptr; // D3D对象
PDIRECT3DDEVICE9 g_D3DDevice = nullptr; // D3D设备对象

/*
int main() {
	createWindow();
	std::cout << "Hello World!\n";
}
*/


LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	default:
		break;
	}

	// windows消息的默认处理函数
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


bool InitializeD3D(HWND hWnd) {
	// 显示模式 （以像素为单位的屏幕宽高，刷新频率，surface formt）
	D3DDISPLAYMODE displayMode;

	// 创建D3D对象 获取主显卡硬件信息  最先被创建，最后被释放
	g_D3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_D3D == nullptr) {
		return false;
	}

	// 获取显示模式
	if (FAILED(g_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT/*查询主显卡*/, &displayMode))) {
		return false;
	}

	// D3D 显示参数
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.Windowed = true; // 是否窗口化
	d3dpp.BackBufferFormat = displayMode.Format;  // D3DFMT_X8R8G8B8  表示为32位RGB像素格式 每种颜色用一个字节表示
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // 帧缓冲区交换方式; 可能是COPY可能是FLIP，由设备来确定适合当前情况的方式
	// 创建D3D设备对象 ---- 代表显卡
	if (FAILED(g_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_D3DDevice))) {
		return false;
	}

	return true;
}


void RenderScene() {
	g_D3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	g_D3DDevice->BeginScene();
	// 3D图形数据
	g_D3DDevice->EndScene();

	// 显示backbuffer内容到屏幕
	g_D3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	auto m = new Module();
	auto b = m->load("./modules/GraphicsWinDX.dll");
	auto c = m->create<GraphicsModule>();
	//c->createView();

	HWND hWnd11 = GetDesktopWindow();
	HINSTANCE aaaaa = GetModuleHandle(nullptr);


	auto className = L"AppClass";

	WNDCLASSEX wnd;
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.style = CS_CLASSDC;//CS_VREDRAW | CS_HREDRAW;
	wnd.lpfnWndProc = MsgProc;
	wnd.cbClsExtra = 0;
	wnd.cbWndExtra = 0;
	wnd.hInstance = hInstance;
	wnd.hIcon = nullptr;//LoadIcon(NULL, IDI_APPLICATION);
	wnd.hCursor = nullptr;//LoadCursor(NULL, IDC_ARROW);
	wnd.hbrBackground = nullptr;//static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wnd.lpszMenuName = nullptr;//NULL;
	wnd.lpszClassName = className;
	wnd.hIconSm = nullptr;

	RegisterClassEx(&wnd);

	HWND hWnd = CreateWindowEx(0L, className, L"Window Title", WS_OVERLAPPEDWINDOW, 100, 100, 640, 480, GetDesktopWindow(), nullptr, hInstance, nullptr);
	if (!hWnd) return 0;

	if (InitializeD3D(hWnd)) {
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);

		MSG msg;

		ZeroMemory(&msg, sizeof(msg));

		while (msg.message != WM_QUIT) {
			if (PeekMessage(
				&msg, // 存储消息的结构体指针
				nullptr, // 窗口消息和线程消息都会被处理 
				0, // 消息过滤最小值; 为0时返回所有可用信息
				0, // 消息过滤最大值; 为0时返回所有可用信息
				PM_REMOVE // 指定消息如何处理; 消息在处理完后从队列中移除
			)) {
				TranslateMessage(&msg); // 变换虚拟键消息到字符消息，字符消息被发送到调用线程的消息队列
				DispatchMessage(&msg); // 派发消息到窗口过程
			} else {
				// 渲染场景
				RenderScene();
				GetTickCount();
			}
		}
	}

	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件