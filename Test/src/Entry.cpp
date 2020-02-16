// Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//#define WIN32_LEAN_AND_MEAN

//#include <windows.h>
//#include <tchar.h>
//#include <string>
//#include "shellapi.h"

//#include <iostream>

#include "tests/DepthTestTester.h"
#include "tests/InputTester.h"
#include "tests/RenderPipelineTester.h"
#include "tests/RenderTargetTester.h"
#include "tests/VertexUpdateTester.h"

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


uint32_t value = 0;
RWAtomicLock<false, false> m;
//std::atomic<uint32_t> value(0);
std::atomic<bool> ready(false);    // can be checked without being set
std::atomic_flag winner = ATOMIC_FLAG_INIT;    // always set when checked

int iv = 0;

int fn(int a) {
	iv += a;
	return iv;
}

void zzzzzzzz() {
	for (int i = 0; i < 10; ++i) {
		m.writeLock();
		++value;
		m.writeUnlock();
	}
}

void zzzzzzzz2() {
	for (int i = 0; i < 10; ++i) {
		m.readLock();
		auto z = value;
		m.readUnlock();
	}
}

void count1m(int id) {
	while (!ready) {
		std::this_thread::yield();
	}

	for (int i = 0; i < 100; ++i) {
		m.writeLock();
		zzzzzzzz();
		m.writeUnlock();
	}
};

void count2m(int id) {
	while (!ready) {
		std::this_thread::yield();
	}

	for (int i = 0; i < 100; ++i) {
		m.readLock();
		zzzzzzzz2();
		m.readUnlock();
	}
};

template<typename T = void>
class C0 {
public:
	C0(void(*l)()) :
		_target((uint32_t)l) {
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
	uint64_t _target;
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

template<typename EvtType, typename... Args>
class CB1 : public IEventListener<EvtType> {
public:
	virtual void AE_CALL onEvent(Event<EvtType>& e) override {
	}
};

template<typename EvtType>
class CB1<EvtType, EvtFn<EvtType>> : public IEventListener<EvtType> {
public:
	virtual void AE_CALL onEvent(Event<EvtType>& e) override {
	}
};

void CBBBBBB(Event<ApplicationEvent>& e) {
	int a = 1;
}

void CBBBBBB222(Event<ApplicationEvent>& e) {
	int a = 1;
}


template<typename T>
struct has_toString {
	template<typename P, std::string (P::*k)()> struct detector {};
	template<typename P> static char func(detector<P, &P::toString>*);
	template<typename P> static long func(...);
	static constexpr bool value = sizeof(func<T>(nullptr)) == sizeof(char);
};


template<typename A>
class Class1 {

};


[[deprecated]]
void mmmmmmm(const Matrix34& m) {

}


template<uint32_t Size>
struct UUII {
	using type = uint64_t;
};

template<>
struct UUII<2> {
	using type = uint16_t;
};

//#include <version>
#include <type_traits>
//#include <bit>

#if __cpp_char8_t
#define ASDF 1
#endif

#include <any>
#include <immintrin.h>
//#include <type_traits>

template<typename T>
struct isFunctor : std::false_type {

};

template<typename L, typename R, typename... Args>
struct isFunctor<R(L::*)(Args...)> : std::true_type {

};

template<typename L>
struct isLambda : isFunctor<decltype(&L::operator())> {
};

template<typename T, typename = std::enable_if<isLambda<T>::value>>
void ssssssss(T fn) {
	T fff = fn;
	fff();
}


template<typename EvtType, typename Fn, typename = std::enable_if_t<std::is_invocable_r<void, Fn, Event<EvtType>>::value, Fn>>
struct AAAAAA {
	using type = Event<EvtType>;
	using ffff = Fn;
};


template<typename EvtType, typename Fn, typename = std::enable_if_t<std::is_invocable_r<void, Fn, Event<EvtType>>::value, Fn>>
class AE_TEMPLATE_DLL EventListener11 : public IEventListener<EvtType> {
public:
	EventListener11(EvtType e, const Fn& fn) :
		_fn(fn) {}

	virtual void AE_CALL onEvent(Event<EvtType>& e) override {
		int a = 1;
		_fn(e);
	}
private:
	Fn _fn;
};



template<typename EvtType, typename Fn, typename = std::enable_if_t<std::is_invocable_r<void, Fn, Event<EvtType>>::value, Fn>>
EventListener11(EvtType, Fn) -> EventListener11<EvtType, Fn>;

template<typename EvtType, typename Fn>
using EventListener22 = EventListener11<EvtType, Fn>;

//template<typename Evt>
//using EventListener11 = EventListener11<Evt, int>;

template<typename Evt, typename Fn>
constexpr bool HHH = std::is_invocable_r<void, Fn, Event<Evt>>::value;


template<size_t Bytes>
decltype(1) testttt() {
	bool b = std::integral_constant<size_t, Bytes>::value >= std::integral_constant<size_t, 4>::value;
	return 0;
}

template<size_t Bytes>
inline constexpr f64 floatMax() {
	static_assert(false, "");
}
template<>
inline constexpr f64 floatMax<4>() {
	return (std::numeric_limits<f32>::max)();
}
template<>
inline constexpr f64 floatMax<8>() {
	return (std::numeric_limits<f64>::max)();
}

class CAAAAA {
public:
	void calllll(Event<ApplicationEvent>& e) {
	}
};


std::unordered_map<std::string, std::any> mappp;

template<typename T>
void adddd(const std::string& name, const T& val) {
	if constexpr (std::is_convertible_v<T, char const*> || std::is_same_v<T, std::string_view>) {
		adddd(name, std::string(val));
	} else {
		auto itr = mappp.find(name);
		if (itr == mappp.end()) {
			mappp.emplace(name, val);
		} else {
			itr->second = val;
		}
	}
}



//int main(int argc, char* argv[]) {
//	HMODULE HIn = GetModuleHandle(NULL);
//	FreeConsole();
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
#if AE_OS == AE_OS_WIN
	SetDllDirectoryW((getAppPath() + L"libs/").data());
#endif
	/*
	abc
	*/

	//new EventListener11(1);

	std::string strr = "123456789";
	//println(String::toString(hash::CRC::calc<64>((uint8_t*)strr.data(), strr.size(), 0xFFFFFFFFFFFFFFFFui64, 0xFFFFFFFFFFFFFFFFui64, true, true, hash::CRC::createTable<64>(0x42F0E1EBA9EA3693ui64)), 16));
	

	/*
	__declspec(align(16)) float a[] = { 1.5, 2.5, 3.5, 4.5 };
	__declspec(align(16)) float b[] = { 1.2, 2.3, 3.4, 4.5 };
	__declspec(align(16)) float c[] = { 0.0, 0.0, 0.0, 0.0 };

	
	__m128 m128_a = _mm_load_ps(a);
	__m128 m128_b = _mm_load_ps(b);
	__m128 m128_c = _mm_add_ps(m128_a, m128_b);

	_mm_store_ps(c, m128_c);

	for (int i = 0; i < 4; i++) {
		printf("%f ", c[i]);
	}
	printf("\n");
	*/
	
	auto v1 = Vec2i32(1, 2, 3, 4, 5.0f);
	
	auto& vvv = v1.slice();

	///*
	auto t0 = Time::now();

	std::vector<std::thread> threads;
	for (int i = 1; i <= 200; ++i) threads.emplace_back(std::thread(count1m, i));
	for (int i = 1; i <= 200; ++i) threads.emplace_back(std::thread(count2m, i));
	ready = true;

	for (auto & th : threads) th.join();

	println(value, "  ", Time::now() - t0, "  ", sizeof(std::atomic<std::thread::id>));

	//std::unordered_map<int, C1> map;
	//map.emplace(1, C1());
	//map.emplace(std::piecewise_construct,
		//std::forward_as_tuple(1),
		//std::forward_as_tuple());
	Matrix44 m1;
	mmmmmmm(m1);
	
	//return (new DepthTestTester())->run();
	//return (new InputTest())->run();
	return (new RenderPipelineTester())->run();
	//return (new RenderTargetTester())->run();
	//return (new VertexUpdateTester())->run();
}