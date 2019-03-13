#pragma once

#define AE_NS       aurora
#define AE_NS_BEGIN namespace AE_NS {
#define AE_NS_END   };
#define AE_FULL_NS  AE_NS
#define USING_AE_NS using namespace AE_FULL_NS;

#define AE_MODULE_NS       AE_NS::module
#define AE_MODULE_NS_BEGIN AE_NS_BEGIN namespace module {
#define AE_MODULE_NS_END   }; AE_NS_END
#define USING_AE_MODULE_NS using namespace AE_MODULE_NS;

#define AE_MODULE_GRAPHICS_NS       AE_MODULE_NS::graphics
#define AE_MODULE_GRAPHICS_NS_BEGIN AE_MODULE_NS_BEGIN namespace graphics {
#define AE_MODULE_GRAPHICS_NS_END   }; AE_MODULE_NS_END
#define USING_AE_MODULE_GRAPHICS_NS using namespace AE_MODULE_GRAPHICS_NS;

#define AE_NODE_NS       AE_NS::node
#define AE_NODE_NS_BEGIN AE_NS_BEGIN namespace node {
#define AE_NODE_NS_END   }; AE_NS_END
#define USING_AE_NODE_NS using namespace AE_NODE_NS;

#define AE_NODE_COMPONENT_NS       AE_NODE_NS::component
#define AE_NODE_COMPONENT_NS_BEGIN AE_NODE_NS_BEGIN namespace component {
#define AE_NODE_COMPONENT_NS_END   }; AE_NODE_NS_END
#define USING_AE_NODE_COMPONENT_NS using namespace AE_NODE_COMPONENT_NS;


#define AE_OS_PLATFORM_UNKNOWN 0
#define AE_OS_PLATFORM_WIN32   1
#define AE_OS_PLATFORM_MAC     2

#define AE_TARGET_OS_PLATFORM  AE_OS_PLATFORM_UNKNOWN

#if defined(_WIN32) || defined(_WINDLL)
#undef  AE_TARGET_OS_PLATFORM
#define AE_TARGET_OS_PLATFORM  AE_OS_PLATFORM_WIN32
#endif


#define AE_CALL __fastcall


#if AE_TARGET_OS_PLATFORM == AE_OS_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef _MSC_VER
#define sleep(ms) Sleep(ms * 1000)
#endif

#elif AE_TARGET_OS_PLATFORM == AE_OS_PLATFORM_MAC;
#include <unistd.h>
#endif


#ifdef __GNUC__
#define AE_DLL_EXPORT __attribute__((dllexport))
#define AE_DLL_IMPORT __attribute__((dllimport))
#else
#define AE_DLL_EXPORT __declspec(dllexport)
#define AE_DLL_IMPORT __declspec(dllimport)
#endif

#define AE_MODULE_DLL_EXPORT AE_DLL_EXPORT
#define AE_MODULE_DLL_IMPORT AE_DLL_IMPORT

#ifdef AE_EXPORTS
#define AE_DLL AE_DLL_EXPORT
#else
#define AE_DLL AE_DLL_IMPORT
#endif

#ifdef AE_MODULE_EXPORTS
#define AE_MODULE_DLL AE_MODULE_DLL_EXPORT
#else
#define AE_MODULE_DLL AE_MODULE_DLL_IMPORT
#endif


typedef char	           i8;
typedef unsigned char      ui8;
typedef short              i16;
typedef unsigned short     ui16;
typedef int                i32;
typedef unsigned int       ui32;
typedef long long          i64;
typedef unsigned long long ui64;
typedef float              f32;
typedef double             f64;

namespace aurora {
	template<typename T>
	inline constexpr ui32 nsizeof(ui32 n) {
		return n * sizeof(T);
	}

	template<typename T>
	inline constexpr ui32 nsizeof(T v, ui32 n) {
		return n * sizeof(v);
	}

	inline void sleeps(ui32 s) {
#ifdef _MSC_VER
		Sleep(s * 1000);
#else
		sleep(s);
#endif
	}

	inline void sleepms(ui32 ms) {
#ifdef _MSC_VER
		Sleep(ms);
#else
		usleep(ms * 1000);
#endif
	}

	inline void sleepus(ui32 us) {
#ifdef _MSC_VER
		Sleep(DWORD(us * .000f));
#else
		usleep(us);
#endif
	}
}