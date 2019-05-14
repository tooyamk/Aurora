#pragma once

#include "Input.h"

#ifdef AE_MODULE_EXPORTS
namespace aurora::modules {
	extern "C" AE_MODULE_DLL_EXPORT void* AE_CREATE_MODULE_FN_NAME(const Args* args) {
		if (!args) {
			println("XInputModule create error : no args");
			return nullptr;
		}

		auto app = args->get<Application*>("app", nullptr);
		if (!app) println("XInputModule create error : no app");

		return new win_xinput::Input(app);
	}
}
#endif