// Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//#define WIN32_LEAN_AND_MEAN

//#include <windows.h>
//#include <tchar.h>
//#include <string>
//#include "shellapi.h"

//#include <iostream>


#include "Aurora.h"

using namespace aurora;
using namespace aurora::events;
using namespace aurora::nodes;
using namespace aurora::modules;
using namespace aurora::modules::graphics;
//using namespace aurora::renderers;
//using namespace aurora::renderers::commands;

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
#include <any>


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

class ICommand {
public:
	virtual ~ICommand() {};
	void execute() {
		_fn(this);
	}

protected:
	ICommand() {};
	void(*_fn)(void*) = nullptr;
};

template<typename T>
class AbstractCommand : public ICommand {
public:
protected:
	AbstractCommand() {
		_fn = &T::execute;
	}
	virtual ~AbstractCommand() {};
};

class Test1Command : public AbstractCommand<Test1Command>{
public:
	Test1Command(int a) :
		_v(a) {
	}
protected:
	int _v = 1;
public:
	static void execute(void* cmd) {
		println("%d", ((Test1Command*)cmd)->_v);
	}
};


int tttt = 0;

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

	Test1Command cmd1(123);
	ICommand* cmd = &cmd1;

	auto size = sizeof(cmd1);

	cmd->execute();

	auto monitors = Monitor::getMonitors();
	auto vms = monitors[0].getVideoModes();

	auto app = new Application(u8"TestApp", 1000000000. / 60.);
	RefGuard appGuard(app);

	if (app->createWindow(Application::Style(), u8"Window Title", Rect<i32>(100, 100, 900, 700), false)) {
		auto ml = new ModuleLoader();
		RefGuard moduleGuard(ml);
		if (ml->load(u8"./modules/GraphicsWinGlew.dll")) {
			auto graphics = ml->create<GraphicsModule>();
			if (graphics) {
				RefGuard graphicsGuard(graphics);
				
				if (graphics->createDevice(app)) {
					auto vb = graphics->createVertexBuffer();
					float vertices[6] = {
						0.0f, 0.0f,
						0.0f, 0.5f,
						0.8f, 0.0f};
					vb->stroage(24, vertices);

					auto p = graphics->createProgram();

					auto appUpdateListener = new EventListenerFunc<Application::Event>(std::bind([app, graphics, vb, p](Event<Application::Event>& e) {
						auto dt = f64(*((i64*)e.getData())) * 0.000001;
						println("%lf", dt);

						if (++tttt >= 120) {
							tttt = 0;
							//app->toggleFullscreen();
						}

						graphics->beginRender();
						graphics->clear();

						vb->use();
						p->use();

						graphics->endRender();
						graphics->present();
					}, std::placeholders::_1));
					RefGuard listenerGuard(appUpdateListener);

					auto evtDispatcher = new EventDispatcher<Application::Event, EmptyLock>();
					RefGuard evtDispatcherGuard(evtDispatcher);

					evtDispatcher->addEventListener(Application::Event::UPDATE, *appUpdateListener, true);
					app->setEventDispatcher(evtDispatcher);
					app->setVisible(true);
					app->run();
				}
			}
		}
	}

	return 0;
}