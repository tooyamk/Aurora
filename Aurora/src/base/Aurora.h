#pragma once

#define AE_OS_PLATFORM_UNKNOWN 0
#define AE_OS_PLATFORM_WIN32   1
#define AE_OS_PLATFORM_MAC     2

#define AE_TARGET_OS_PLATFORM  AE_OS_PLATFORM_UNKNOWN

#if defined(_WIN32) || defined(_WINDLL)
#undef  AE_TARGET_OS_PLATFORM
#define AE_TARGET_OS_PLATFORM  AE_OS_PLATFORM_WIN32
#endif


#define AE_CALL __fastcall


#define ae_internal_public public


#if AE_TARGET_OS_PLATFORM == AE_OS_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "atlbase.h"
#include "atlstr.h"

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

#define AE_TEMPLATE_DLL_EXPORT AE_DLL_EXPORT
#define AE_TEMPLATE_DLL_IMPORT

#ifdef AE_EXPORTS
#define AE_DLL AE_DLL_EXPORT
#define AE_TEMPLATE_DLL AE_TEMPLATE_DLL_EXPORT
#else
#define AE_DLL AE_DLL_IMPORT
#define AE_TEMPLATE_DLL AE_TEMPLATE_DLL_IMPORT
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
	static void print(const i8* msg, va_list args) {
#if AE_TARGET_OS_PLATFORM == AE_OS_PLATFORM_WIN32
		i8 strBuffer[4096] = { 0 };
		_vsnprintf_s(strBuffer, sizeof(strBuffer) - 1, msg, args);
		OutputDebugString(CA2W(strBuffer));
#endif
	}

	static void print(const i8* msg, ...) {
		va_list args;
		va_start(args, msg);
		print(msg, args);
		va_end(args);
	}

	static void println(const i8* msg, ...) {
		va_list args;
		va_start(args, msg);
		print(msg, args);
		va_end(args);

		print("\n");
	}
}