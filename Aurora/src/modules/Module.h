#pragma once

#include "base/DynamicLib.h"
#include "base/Ref.h"

AE_MODULE_NS_BEGIN

typedef void*(*AE_CREATE_MODULE_FUN)();

class AE_TEMPLATE_DLL Module : public Ref {
public:
	Module() : _createFn(nullptr) {}

	bool AE_CALL load(const i8* path) {
		if (_lib.isLoaded()) _lib.free();
		if (_lib.load(path)) {
			_createFn = (AE_CREATE_MODULE_FUN)_lib.getSymbolAddress("createModule");
			return _createFn;
		} else {
			_createFn = nullptr;
			return false;
		}
		return false;
	}

	void AE_CALL free() {
		_lib.free();
	}

	template<class T>
	T* AE_CALL create() const {
		if (_createFn && _lib.isLoaded()) {
			return (T*)_createFn();
		} else {
			return nullptr;
		}
	}

private:
	DynamicLib _lib;
	AE_CREATE_MODULE_FUN _createFn;
};

AE_MODULE_NS_END