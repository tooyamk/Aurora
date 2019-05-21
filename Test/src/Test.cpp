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
using namespace aurora::modules::inputs;
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


i8* printGamepadKey(GamepadKeyCode code) {
	switch (code) {
	case GamepadKeyCode::LEFT_STICK:
		return "left stick";
	case GamepadKeyCode::RIGHT_STICK:
		return "right stick";
	case GamepadKeyCode::LEFT_THUMB:
		return "left thumb";
	case GamepadKeyCode::RIGHT_THUMB:
		return "right thumb";
	case GamepadKeyCode::DPAD:
		return "dpad";
	case GamepadKeyCode::LEFT_SHOULDER:
		return "left shoulder";
	case GamepadKeyCode::RIGHT_SHOULDER:
		return "right shoulder";
	case GamepadKeyCode::LEFT_TRIGGER:
		return "left trigger";
	case GamepadKeyCode::RIGHT_TRIGGER:
		return "right trigger";
	case GamepadKeyCode::SELECT:
		return "select";
	case GamepadKeyCode::START:
		return "start";
	case GamepadKeyCode::A:
		return "a";
	case GamepadKeyCode::B:
		return "b";
	case GamepadKeyCode::X:
		return "x";
	case GamepadKeyCode::Y:
		return "y";
	case GamepadKeyCode::TOUCH_PAD:
		return "touch pad";
	default:
		return "undefined";
	}
}


template<typename T>
void aabbccdsd(const T& v) {
	bool b0 = std::is_same_v<T, i8 const*>;
	bool b1 = std::is_convertible_v<T, i8 const*>;
	bool b2 = std::is_pointer_v<T>;
	bool b3 = std::is_array_v<T>;
	auto ss = typeid(v).name();
	int a = 1;
}

class IINNN {
public:
	virtual std::string toString() {
		return "";
	}
};

class BBCDCC : public IINNN {

};

template<typename T>
struct has_toString {
	template<typename P, std::string (P::*k)()> struct detector {};
	template<typename P> static char func(detector<P, &P::toString>*);
	template<typename P> static long func(...);
	static constexpr bool value = sizeof(func<T>(nullptr)) == sizeof(char);
};


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

	auto sdfwe = *(ui16*)"\0\xFF" == 0xFF00;

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


		RefPtr<InputModuleLoader> directInputModuleLoader = new InputModuleLoader();
		directInputModuleLoader.get()->load(u8"./modules/WinDirectInput.dll");
		RefPtr<InputModuleLoader> xInputModuleLoader = new InputModuleLoader();
		xInputModuleLoader.get()->load(u8"./modules/WinXInput.dll");
		if (gml->load(u8"./modules/WinGlew.dll")) {
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
			std::vector<RefPtr<IInputModule>> inputModules;
			if (auto im = xInputModuleLoader.get()->create(&Args().add("app", app)); im) inputModules.emplace_back(im);
			if (auto im = directInputModuleLoader.get()->create(&Args().add("app", app)); im) inputModules.emplace_back(im);
			if (graphics) {
				println("Graphics Version : ", graphics->getVersion().c_str());

				RefPtr<IGraphicsModule> graphicsGuard(graphics);

				std::vector<IInputDevice*> inputDevices;

				for (auto& im : inputModules) {
					im.get()->getEventDispatcher().addEventListener(ModuleEvent::CONNECTED, *new EventListenerFunc<ModuleEvent>(std::bind([&inputDevices, app](Event<ModuleEvent>& e) {
						auto getNumInputeDevice = [&inputDevices](DeviceType type) {
							ui32 n = 0;
							for (auto& dev : inputDevices) {
								if (dev->getInfo().type == type) ++n;
							}
							return n;
						};

						auto info = (DeviceInfo*)e.getData();
						if ((info->type & DeviceType::KEYBOARD | DeviceType::GAMEPAD) != DeviceType::UNKNOWN) {
							auto im = (IInputModule*)e.getTarget();
							if (getNumInputeDevice(DeviceType::GAMEPAD) > 0) return;
							println("create device : ", (ui32)info->type, " guid size = ", info->guid.getSize());
							auto device = im->createDevice(info->guid);
							if (device) {
								device->getEventDispatcher().addEventListener(DeviceEvent::DOWN, *new EventListenerFunc<DeviceEvent>(std::bind([app](Event<DeviceEvent>& e) {
									auto device = (IInputDevice*)e.getTarget();
									switch (device->getInfo().type) {
									case DeviceType::KEYBOARD:
									{
										auto key = (Key*)e.getData();
										if (key->code == (ui32)KeyboardVirtualKeyCode::KEY_ENTER) {
											f32 status;
											if (((IInputDevice*)e.getTarget())->getKeyState((ui8)KeyboardVirtualKeyCode::KEY_RCTRL, &status, 1) && status != 0.f) {
												app->toggleFullscreen();
											}
										}

										break;
									}
									case DeviceType::GAMEPAD:
									{
										auto key = (Key*)e.getData();
										println("gamepad down : ", printGamepadKey((GamepadKeyCode)key->code), "  ", key->value[0]);
										if (key->code == (ui32)GamepadKeyCode::CROSS) {
											device->setVibration(0.5f, 0.5f);
										}

										break;
									}
									}
									//println("input device down : %d  %f", key->code, key->value[0]);
								}, std::placeholders::_1)), true);

								device->getEventDispatcher().addEventListener(DeviceEvent::UP, *new EventListenerFunc<DeviceEvent>(std::bind([](Event<DeviceEvent>& e) {
									auto device = (IInputDevice*)e.getTarget();
									switch (device->getInfo().type) {
									case DeviceType::KEYBOARD:
									{
										break;
									}
									case DeviceType::GAMEPAD:
									{
										auto key = (Key*)e.getData();
										println("gamepad up : ", printGamepadKey((GamepadKeyCode)key->code), "  ", key->value[0]);
										if (key->code == (ui32)GamepadKeyCode::CROSS) {
											device->setVibration(0.0f, 0.0f);
										}

										break;
									}
									}
								}, std::placeholders::_1)), true);

								device->getEventDispatcher().addEventListener(DeviceEvent::MOVE, *new EventListenerFunc<DeviceEvent>(std::bind([](Event<DeviceEvent>& e) {
									switch (((IInputDevice*)e.getTarget())->getInfo().type) {
									case DeviceType::MOUSE:
									{
										auto key = (Key*)e.getData();
										if (key->code == 4) {
											//f32 curPos[2];
											//((InputDevice*)e.getTarget())->getKeyState(key->code, curPos, 2);
											println("input device move : ", key->code, key->value[0], key->value[1], key->value[2]);
										} else if (key->code == 1) {
											//println("input device wheel : %d   %f", key->code, *(f32*)key->value);
										}

										break;
									}
									case DeviceType::GAMEPAD:
									{
										auto key = (Key*)e.getData();
										print("gamepad move : ", printGamepadKey((GamepadKeyCode)key->code), " ", key->value[0]);
										if (key->count > 1) print("  ", key->value[1]);
										println();

										break;
									}
									}

								}, std::placeholders::_1)), true);

								device->ref();
								inputDevices.emplace_back(device);
							}
						}
						println("input device connected : ", info->type);
					}, std::placeholders::_1)), true);

					im.get()->getEventDispatcher().addEventListener(ModuleEvent::DISCONNECTED, *new EventListenerFunc<ModuleEvent>(std::bind([&inputDevices](Event<ModuleEvent>& e) {
						auto info = (DeviceInfo*)e.getData();
						for (ui32 i = 0, n = inputDevices.size(); i < n; ++i) {
							if (inputDevices[i]->getInfo().guid == info->guid) {
								inputDevices[i]->unref();
								inputDevices.erase(inputDevices.begin() + i);
								break;
							}
						}
						println("input device disconnected : ", info->type);
					}, std::placeholders::_1)), true);
				}

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
					vertexBuffer->create(sizeof(vertices), Usage::MAP_WRITE | Usage::UPDATE | Usage::PERSISTENT_MAP, vertices, sizeof(vertices));
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
				f32 val2[] = { 1.0f, 1.0f };
				aabbccStruct->add("val2", new ShaderParameter(ShaderParameterUsage::EXCLUSIVE))->set<f32>(val2, sizeof(val2), sizeof(f32), true).setUpdated();
				aabbccStruct->add("val3", new ShaderParameter())->set(Vec4f32::ONE).setUpdated();
				cf->add("blue", new ShaderParameter())->set(aabbccStruct);

				auto texRes = graphics->createTexture2DResource();
				if (texRes) {
					auto texView = graphics->createTextureView();
					if (texView) texView->create(texRes, 0, -1, 0, -1);

					auto img0 = file::PNGConverter::parse(readFile(app->getAppPath() + u8"../../Resources/c4.png"));
					auto mipLevels = Image::calcMipLevels(img0->size);
					ByteArray mipsData0;
					std::vector<void*> mipsData0Ptr;
					img0->generateMips(img0->format, mipLevels, mipsData0, mipsData0Ptr);

					auto img1 = file::PNGConverter::parse(readFile(app->getAppPath() + u8"../../Resources/red.png"));
					ByteArray mipsData1;
					std::vector<void*> mipsData1Ptr;
					img1->generateMips(img1->format, mipLevels, mipsData1, mipsData1Ptr);

					mipsData0Ptr.insert(mipsData0Ptr.end(), mipsData1Ptr.begin(), mipsData1Ptr.end());

					texRes->create(img0->size, 0, 1, img0->format, Usage::UPDATE, mipsData0Ptr.data());
					//auto mapped = tex->map(Usage::CPU_WRITE);
					ui8 texData[] = { 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 0, 0, 0, 255 };
					//texRes->write(0, Rect<ui32>(2, 3, 3, 4), texData);
					//tex->unmap();
					//texRes->map(0, 0, Usage::MAP_WRITE);
					//texRes->update(0, 0, Box2ui32(Vec2ui32(0, 0), Vec2ui32(2, 2)), texData);

					cf->add("texDiffuse", new ShaderParameter(ShaderParameterUsage::AUTO))->set(texView ? (ITextureViewBase*)texView : (ITextureViewBase*)texRes).setUpdated();
				}

				auto sam = graphics->createSampler();
				if (sam) {
					sam->setMipLOD(0, 0);
					sam->setFilter(SamplerFilterOperation::NORMAL, SamplerFilterMode::POINT, SamplerFilterMode::POINT, SamplerFilterMode::POINT);
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

				evtDispatcher.addEventListener(ApplicationEvent::UPDATE, *new EventListenerFunc<ApplicationEvent>(std::bind([graphics, &inputModules, &inputDevices, vbf, cf, p, ib](Event<ApplicationEvent>& e) {
					auto app = (Application*)e.getTarget();

					auto dt = f64(*((i64*)e.getData())) * 0.000001;

					for (auto& im : inputModules) im.get()->poll();
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

							auto vb = vbf->get("POSITION0");
							if (vb) {
								auto cycle = 20000;
								auto halfCycyle = f32(cycle / 2);
								auto t = Time::now<std::chrono::milliseconds>() % cycle;
								f32 vertices[] = {
									0.f, 0.f,
									0.f, 1.f,
									1.f, 1.f,
									1.f, 0.f };
								f32 v = t <= halfCycyle ? 1.f - f32(t) / halfCycyle : (f32(t) - halfCycyle) / halfCycyle;
								vertices[3] = v;
								if ((vb->map(Usage::MAP_WRITE | Usage::MAP_SWAP) & Usage::DISCARD) == Usage::DISCARD) {
									vb->write(0, vertices, sizeof(vertices));
								} else {
									vb->write(12, &v, 4);
								}
								//vb->update(12, &v, 4);
								vb->unmap();
							}

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