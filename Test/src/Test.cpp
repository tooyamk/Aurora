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
#include "math/Math.h"
#include "modules/Module.h"
#include "modules/graphics/GraphicsModule.h"
#include "utils/String.h"
#include "math/Rect.h"
#include "events/EventDispatcher.h"
#include "base/Lock.h"
#include "base/Application.h"
#include "base/Time.h"

using namespace aurora;
using namespace aurora::event;
using namespace aurora::node;
using namespace aurora::module;
using namespace aurora::module::graphics;

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

#include <unordered_map>
#include <vector>
#include <functional>
#include <atomic>
#include <thread>
#include <assert.h>
#include <mutex>


ui32 value = 0;
RecursiveAtomicLock m;
//std::atomic<ui32> value(0);
std::atomic<bool> ready(false);    // can be checked without being set
std::atomic_flag winner = ATOMIC_FLAG_INIT;    // always set when checked

int iv = 0;

int fn(int a) {
	iv += a;
	return iv;
}

void zzzzzzzz() {
	m.lock();
	for (int i = 0; i < 10000; ++i) {
		++value;
	} // 计数.
	m.unlock();
}

void count1m(int id) {
	while (!ready) {
		std::this_thread::yield();
	}

	m.lock();
	for (int i = 0; i < 100; ++i) {
		zzzzzzzz();
	} // 计数.
	m.unlock();
	// 如果某个线程率先执行完上面的计数过程，则输出自己的 ID.
	// 此后其他线程执行 test_and_set 是 if 语句判断为 false，
	// 因此不会输出自身 ID.
};

template<typename T = void>
class C0 {
public:
	C0(void(*l)()) :
		_target((ui32)l) {
	}

	template<bool b>
	void call() {
		println("other");
		((void(*)())_target)();
	}

	template<>
	void call<true>() {
		println("void");
		((void(*)())_target)();
	}

	void operator()() {
		println("T is void : %d", sizeof(T));
		//call<std::is_void_v<T>>();
	}

protected:
	bool bbb = std::is_void_v<T>;
	ui64 _target;
};

#define ae_internal_public public

class C1 {
ae_internal_public:
	C1() {
		println("init");
	}
	C1(C1&& c) {
		println("move init");
	}
	~C1() {
		println("c1");
	}

	void aaa(Event<Application::Event>& e) {
		++iv;
	}

private:
	int ttt = 2;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	///*
	std::vector<std::thread> threads;
	for (int i = 1; i <= 8; ++i)
		threads.push_back(std::thread(count1m, i));
	ready = true;

	for (auto & th : threads) th.join();
	//*/

	//std::unordered_map<int, C1> map;
	//map.emplace(1, C1());
	//map.emplace(std::piecewise_construct,
		//std::forward_as_tuple(1),
		//std::forward_as_tuple());


	WNDCLASSEXW style;
	style.cbSize = 0;
	style.style = CS_CLASSDC;//CS_VREDRAW | CS_HREDRAW;
	style.lpfnWndProc = nullptr;
	style.cbClsExtra = 0;
	style.cbWndExtra = 0;
	style.hInstance = nullptr;
	style.hIcon = nullptr;//LoadIcon(NULL, IDI_APPLICATION);
	style.hCursor = nullptr;//LoadCursor(NULL, IDC_ARROW);
	style.hbrBackground = nullptr;//static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	style.lpszMenuName = nullptr;//NULL;
	style.lpszClassName = L"AppClass";
	style.hIconSm = nullptr;

	auto m = new Module();
	auto b = m->load("./modules/GraphicsWinD3D9.dll");
	auto c = m->create<GraphicsModule>();
	if (c->createView(&style, u8"Window Title", Rect<i32>(100, 100, 740, 580), false)) {
		std::function<void(Event<Application::Event>&)> fffffff = std::bind([c](Event<Application::Event>& e) {
			auto dt = f64(*((i64*)e.getData())) * 0.000001;
			println("%lf", dt);
			c->beginRender();
			c->clear();
			c->endRender();
			c->present();
		}, std::placeholders::_1);
		FuncEventListener<Application::Event> enterFrameHandler(fffffff);

		Application app(1000000000. / 60.);
		EventDispatcher<Application::Event, EmptyLock> ed;
		ed.addEventListener(Application::Event::UPDATE, enterFrameHandler);
		app.setEventDispatcher(&ed);
		app.run();
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