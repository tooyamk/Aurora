// Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//#define WIN32_LEAN_AND_MEAN

//#include <windows.h>
//#include <tchar.h>
//#include <string>
//#include "shellapi.h"

//#include <iostream>


#include "base/Aurora.h"
#include "base/Ref.h"
#include "nodes/Node.h"
#include "math/Matrix44.h"
#include "base/Console.h"
#include "math/Math.h"
#include "modules/graphics/GraphicsModule.h"
#include "utils/String.h"
#include "math/Rect.h"

USING_AE_NS
USING_AE_MODULE_NS
USING_AE_MODULE_GRAPHICS_NS

/*
int main() {
	createWindow();
	std::cout << "Hello World!\n";
}
*/

/*
void RenderScene() {
	g_D3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	g_D3DDevice->BeginScene();
	// 3D图形数据
	g_D3DDevice->EndScene();

	// 显示backbuffer内容到屏幕
	g_D3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
}
*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEXW wnd;
	wnd.cbSize = 0;
	wnd.style = CS_CLASSDC;//CS_VREDRAW | CS_HREDRAW;
	wnd.lpfnWndProc = nullptr;
	wnd.cbClsExtra = 0;
	wnd.cbWndExtra = 0;
	wnd.hInstance = nullptr;
	wnd.hIcon = nullptr;//LoadIcon(NULL, IDI_APPLICATION);
	wnd.hCursor = nullptr;//LoadCursor(NULL, IDC_ARROW);
	wnd.hbrBackground = nullptr;//static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wnd.lpszMenuName = nullptr;//NULL;
	wnd.lpszClassName = L"AppClass";
	wnd.hIconSm = nullptr;

	auto m = new Module();
	auto b = m->load("./modules/GraphicsWinD3D9.dll");
	auto c = m->create<GraphicsModule>();
	c->createView(&wnd, u8"Window Title" , Rect<i32>(100, 100, 740, 580), false, 60.f);

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