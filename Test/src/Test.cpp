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

#include "../../Extensions/Files/Images/PNGConverter/src/PNGConverter.h"
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
#include <fstream>
#include <bitset>


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
		println("T is void : ", sizeof(T));
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

	void* operator new(size_t size) {
		println("new 0");
		return malloc(size);
	}

protected:
	ICommand() {};
	void(*_fn)(void*) = nullptr;
};

template<typename T>
class AbstractCommand : public ICommand {
public:
	AbstractCommand() {
		_fn = &T::execute;
	}
	void* operator new(size_t size) {
		println("new 1");
		return malloc(size);
	}
protected:
	
	virtual ~AbstractCommand() {};
};

class Test1Command : public AbstractCommand<Test1Command>{
public:
	Test1Command(int a) :
		_v(a) {
	}

	void* operator new(size_t size) {
		println("new 2");
		return malloc(size);
	}
protected:
	int _v = 1;
public:
	static void execute(void* cmd) {
		println("%d", ((Test1Command*)cmd)->_v);
	}
};

int tttt = 0;

template<typename M>
class PPPP {
public:
};

ByteArray readFile(const std::string& path) {
	ByteArray dst;
	std::ifstream stream(path, std::ios::in | std::ios::binary);
	if (stream.good()) {
		auto beg = stream.tellg();
		stream.seekg(0, std::ios::end);
		auto end = stream.tellg();
		auto size = end - beg;

		auto data = new i8[size];

		stream.seekg(0, std::ios::beg);
		stream.read(data, size);

		dst = ByteArray(data, size, ByteArray::ExtMemMode::EXCLUSIVE);
	}
	stream.close();
	return std::move(dst);
}

ProgramSource readProgramSourcee(const std::string& path, ProgramStage type) {
	ProgramSource s;
	s.language = ProgramLanguage::HLSL;
	s.stage = type;
	s.data = readFile(path);
	return std::move(s);
}

//int main(int argc, char* argv[]) {
//	HMODULE HIn = GetModuleHandle(NULL);
//	FreeConsole();
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	///*
	std::vector<std::thread> threads;
	for (int i = 1; i <= 8; ++i) threads.push_back(std::thread(count1m, i));
	ready = true;

	for (auto & th : threads) th.join();


	//std::unordered_map<int, C1> map;
	//map.emplace(1, C1());
	//map.emplace(std::piecewise_construct,
		//std::forward_as_tuple(1),
		//std::forward_as_tuple());

	auto monitors = Monitor::getMonitors();
	auto vms = monitors[0].getVideoModes();

	auto app = new Application(u8"TestApp", 1000000000. / 60.);
	RefPtr<Application> appGuard(app);

	SetDllDirectory(String::Utf8ToUnicode(app->getAppPath() + u8"extensions/").c_str());
	//LoadLibrary(String::Utf8ToUnicode(pathh + u8"extensions/PNG.dll").c_str());
	
	Application::Style wndStype;
	wndStype.thickFrame = true;
	if (app->createWindow(wndStype, u8"", Box2i32(Vec2i32({ 100, 100 }), Vec2i32({ 800, 600 })), false)) {
		auto gml = new GraphicsModuleLoader();
		RefPtr<GraphicsModuleLoader> moduleGLGuard(gml);

		/*
		auto d3d11Loader = new GraphicsModuleLoader();
		d3d11Loader->load(u8"./modules/WinD3D11.dll");
		auto d3d11 = d3d11Loader->create(app);
		d3d11->createDevice();
		*/


		auto directInputModuleLoader = new InputModuleLoader();
		RefPtr<InputModuleLoader> moduleKBGuard(directInputModuleLoader);
		if (gml->load(u8"./modules/WinGlew.dll") && directInputModuleLoader->load(u8"./modules/WinDirectInput.dll")) {
			auto gpstml = new ModuleLoader<IProgramSourceTranslator>();
			gpstml->load(u8"./modules/ProgramSourceTranslator.dll");
			auto gpst = gpstml->create(&Args().add("dxc", "dxcompiler.dll"));

			/*
			GraphicsProgramSource source;
			source.language = GraphicsProgramSourceLanguage::HLSL;
			source.type = GraphicsProgramType::VERTEX_PROGRAM;
			source.version = "6.0";
			source.data = readFile(u8"../Resources/vert.hlsl");
			auto s = gpst->translate(source, GraphicsProgramSourceLanguage::MSL, "");
			println("%s", s.data.getBytes());
			*/

			auto graphics = gml->create(&Args().add("app", app).add("trans", gpst));
			auto directInputModule = directInputModuleLoader->create(&Args().add("app", app));
			if (graphics && directInputModule) {
				println("Graphics Version : ", graphics->getVersion().c_str());

				RefPtr<IGraphicsModule> graphicsGuard(graphics);
				RefPtr<IInputModule> kbGuard(directInputModule);

				std::vector<IInputDevice*> inputDevices;

				directInputModule->getEventDispatcher().addEventListener(InputModuleEvent::CONNECTED, *new EventListenerFunc<InputModuleEvent>(std::bind([&inputDevices, app](Event<InputModuleEvent>& e) {
					auto info = (InputDeviceInfo*)e.getData();
					if (info->type & (InputDeviceType::KEYBOARD)) {
						auto di = (IInputModule*)e.getTarget();
						auto device = di->createDevice(info->guid);
						if (device) {
							device->getEventDispatcher().addEventListener(InputDeviceEvent::DOWN, *new EventListenerFunc<InputDeviceEvent>(std::bind([app](Event<InputDeviceEvent>& e) {
								auto key = (InputKey*)e.getData();
								if (key->code == (ui32)KeyboardVirtualKeyCode::KEY_ENTER) {
									f32 status;
									if (((IInputDevice*)e.getTarget())->getKeyState((ui8)KeyboardVirtualKeyCode::KEY_RCTRL, &status, 1) && status != 0.f) {
										app->toggleFullscreen();
									}
									
								}
								//println("input device down : %d  %f", key->code, key->value[0]);
							}, std::placeholders::_1)), true);

							device->getEventDispatcher().addEventListener(InputDeviceEvent::UP, *new EventListenerFunc<InputDeviceEvent>(std::bind([](Event<InputDeviceEvent>& e) {
								auto key = (InputKey*)e.getData();
								//println("input device up : %d  %f", key->code, key->value[0]);
							}, std::placeholders::_1)), true);

							device->getEventDispatcher().addEventListener(InputDeviceEvent::MOVE, *new EventListenerFunc<InputDeviceEvent>(std::bind([](Event<InputDeviceEvent>& e) {
								auto key = (InputKey*)e.getData();
								if (key->code == 4) {
									//f32 curPos[2];
									//((InputDevice*)e.getTarget())->getKeyState(key->code, curPos, 2);
									println("input device move : ", key->code, key->value[0], key->value[1], key->value[2]);
								} else if (key->code == 1) {
									//println("input device wheel : %d   %f", key->code, *(f32*)key->value);
								}
							}, std::placeholders::_1)), true);

							device->ref();
							inputDevices.emplace_back(device);
						}
					}
					println("input device connected : ", info->type);
				}, std::placeholders::_1)), true);

				directInputModule->getEventDispatcher().addEventListener(InputModuleEvent::DISCONNECTED, *new EventListenerFunc<InputModuleEvent>(std::bind([&inputDevices](Event<InputModuleEvent>& e) {
					auto info = (InputDeviceInfo*)e.getData();
					for (ui32 i = 0, n = inputDevices.size(); i < n; ++i) {
						if (inputDevices[i]->getInfo().guid == info->guid) {
							inputDevices[i]->unref();
							inputDevices.erase(inputDevices.begin() + i);
							break;
						}
					}
					println("input device disconnected : ", info->type);
				}, std::placeholders::_1)), true);

				auto vertexBuffer = graphics->createVertexBuffer();
				if (vertexBuffer) {
					/*
					f32 vertices[] = {
						0.0f, 0.0f,
						0.0f, 0.5f,
						0.8f, 0.0f };
					vb->stroage(sizeof(vertices), vertices);
					vb->setFormat(VertexSize::TWO, VertexType::F32);
					*/
					///*
					f32 vertices[] = {
						0.f, 0.f,
						0.f, 1.f,
						1.f, 1.f,
						1.f, 0.f };
					vertexBuffer->create(sizeof(vertices), Usage::CPU_WRITE, vertices, sizeof(vertices));
					vertexBuffer->setFormat(VertexSize::TWO, VertexType::F32);
				}

				auto uvBuffer = graphics->createVertexBuffer();
				if (uvBuffer) {
					f32 uvs[] = {
						0.f, 1.f,
						0.f, 0.f,
						1.f, 0.f,
						1.f, 1.f };
					uvBuffer->create(sizeof(uvs), Usage::NONE, uvs, sizeof(uvs));
					uvBuffer->setFormat(VertexSize::TWO, VertexType::F32);
				}

				auto vbf = new VertexBufferFactory();
				vbf->add("POSITION0", vertexBuffer);
				vbf->add("TEXCOORD0", uvBuffer);

				auto cf = new ShaderParameterFactory();
				cf->add("red", new ShaderParameter(ShaderParameterUsage::EXCLUSIVE))->set(Vec4f32::ONE).setUpdated();
				cf->add("green", new ShaderParameter(ShaderParameterUsage::EXCLUSIVE))->set(Vec4f32::ONE).setUpdated();
				//cf->add("blue", new ShaderParameter())->set(Vec4f32::ONE).setUpdated();

				auto aabbccStruct = new ShaderParameterFactory();
				aabbccStruct->add("val1", new ShaderParameter(ShaderParameterUsage::EXCLUSIVE))->set(Vec4f32::ONE).setUpdated();
				f32 val2[] = { 1.0f, 0.8f };
				aabbccStruct->add("val2", new ShaderParameter(ShaderParameterUsage::EXCLUSIVE))->set<f32>(val2, 16, 4, true).setUpdated();
				aabbccStruct->add("val3", new ShaderParameter())->set(Vec4f32::ONE).setUpdated();
				cf->add("blue", new ShaderParameter())->set(aabbccStruct);

				auto texRes = graphics->createTexture2DResource();
				if (texRes) {
					auto texView = graphics->createTextureView();
					if (texView) texView->create(texRes, 0, -1, 0, -1);

					auto img0 = file::PNGConverter::parse(readFile(app->getAppPath() + u8"../../Resources/white.png"));
					auto mipLevels = Image::calcMipLevels(img0->size);
					ByteArray mipsData0;
					std::vector<void*> mipsData0Ptr;
					img0->generateMips(img0->format, mipLevels, mipsData0, mipsData0Ptr);

					auto img1 = file::PNGConverter::parse(readFile(app->getAppPath() + u8"../../Resources/red.png"));
					ByteArray mipsData1;
					std::vector<void*> mipsData1Ptr;
					img1->generateMips(img1->format, mipLevels, mipsData1, mipsData1Ptr);

					mipsData0Ptr.insert(mipsData0Ptr.end(), mipsData1Ptr.begin(), mipsData1Ptr.end());

					texRes->create(img0->size, 0, 1, img0->format, Usage::GPU_WRITE, mipsData0Ptr.data());
					//auto mapped = tex->map(Usage::CPU_WRITE);
					ui8 texData[] = { 255, 255, 0, 0, 255, 0, 0, 0, 0, 0, 5 };
					//texRes->write(0, Rect<ui32>(2, 3, 3, 4), texData);
					//tex->unmap();

					cf->add("texDiffuse", new ShaderParameter(ShaderParameterUsage::AUTO))->set(texView ? (ITextureViewBase*)texView : (ITextureViewBase*)texRes).setUpdated();
				}

				auto sam = graphics->createSampler();
				if (sam) {
					sam->setFilter(SamplerFilterOperation::NORMAL, SamplerFilterMode::LINEAR, SamplerFilterMode::POINT, SamplerFilterMode::LINEAR);
					cf->add("samLiner", new ShaderParameter(ShaderParameterUsage::AUTO))->set(sam).setUpdated();
				}

				auto ib = graphics->createIndexBuffer();
				if (ib) {
					ui16 indices[] = {
						0, 1, 3,
						3, 1, 2 };
					ib->create(sizeof(indices), Usage::NONE, indices, sizeof(indices));
					ib->setFormat(IndexType::UI16);
				}

				auto p = graphics->createProgram();
				if (p) {
					p->upload(readProgramSourcee(app->getAppPath() + u8"../../Resources/vert.hlsl", ProgramStage::VS), readProgramSourcee(app->getAppPath() + u8"../../Resources/frag.hlsl", ProgramStage::PS));
				}

				auto appClosingListener = new EventListenerFunc<ApplicationEvent>(std::bind([](Event<ApplicationEvent>& e) {
					//*(bool*)e.getData() = true;

				}, std::placeholders::_1));
				RefPtr<Ref> appClosingListenerGuard(appClosingListener);

				auto& evtDispatcher = app->getEventDispatcher();

				evtDispatcher.addEventListener(ApplicationEvent::UPDATE, *new EventListenerFunc<ApplicationEvent>(std::bind([graphics, directInputModule, &inputDevices, vbf, cf, p, ib](Event<ApplicationEvent>& e) {
					auto app = (Application*)e.getTarget();

					auto dt = f64(*((i64*)e.getData())) * 0.000001;

					directInputModule->poll();
					for (auto& dev : inputDevices) dev->poll(true);

					//app->setWindowTitle(String::toString(GetKeyboardType(0)) + "  " + String::toString(GetKeyboardType(1)) + "  " + String::toString(GetKeyboardType(2)));

					//println("%lf", dt);
					//app->setWindowTitle(String::toString(dt));

					if (++tttt >= 120) {
						tttt = 0;
						//app->toggleFullscreen();
					}

					graphics->beginRender();
					graphics->clear();

					if (p) {
						if (p->use()) {
							cf->get("red")->set(Vec4f32((f32)rand() / (f32)RAND_MAX, (f32)rand() / (f32)RAND_MAX)).setUpdated();
							p->draw(vbf, cf, ib);
						}
					}

					graphics->endRender();
					graphics->present();
				}, std::placeholders::_1)), true);
				evtDispatcher.addEventListener(ApplicationEvent::CLOSING, *appClosingListener, true);
				app->setVisible(true);
				app->run();
			}
		}
	}

	return 0;
}