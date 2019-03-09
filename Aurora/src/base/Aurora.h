#pragma once

#define AE_NS       aurora
#define AE_NS_BEGIN namespace AE_NS {
#define AE_NS_END   };
#define USING_AE_NS using namespace AE_NS;

#define AE_OS_PLATFORM_UNKNOWN 0
#define AE_OS_PLATFORM_WIN32   1
#define AE_OS_PLATFORM_MAC     2

#define AE_TARGET_OS_PLATFORM  AE_OS_PLATFORM_UNKNOWN

#if defined(_WIN32) && defined(_WINDOWS)
#undef  AE_TARGET_OS_PLATFORM
#define AE_TARGET_OS_PLATFORM  AE_OS_PLATFORM_WIN32
#endif


#define AE_CALL __fastcall


#if AE_TARGET_OS_PLATFORM == AE_OS_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


#define AE_DLL_EXPORT __declspec(dllexport) 
#define AE_DLL_IMPORT __declspec(dllimport)

#ifdef AE_EXPORTS
#define AE_DLL AE_DLL_EXPORT 
#else
#define AE_DLL AE_DLL_IMPORT 
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
}