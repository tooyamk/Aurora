#pragma once

#include "base/Aurora.h"

AE_NS_BEGIN

class AE_DLL Ref {
public:
	Ref();
	virtual ~Ref();

	inline void AE_CALL retain() {
		++_rc;
	}
	inline void AE_CALL release() {
		if (_rc <= 1) {
			delete this;
		} else {
			--_rc;
		}
	}

	inline ui32 getReferenceCount() const {
		return _rc;
	}

protected:
	ui32 _rc;
};

AE_NS_END
