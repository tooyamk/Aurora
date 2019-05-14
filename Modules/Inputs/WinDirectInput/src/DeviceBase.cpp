#include "DeviceBase.h"
#include "Input.h"

namespace aurora::modules::win_direct_input {
	DeviceBase::DeviceBase(Input* input, LPDIRECTINPUTDEVICE8 dev, const InputDeviceInfo& info) :
		_input(input->ref<Input>()),
		_dev(dev),
		_info(info) {
	}

	DeviceBase::~DeviceBase() {
		_dev->Unacquire();
		_dev->Release();
		Ref::setNull<Input>(_input);
	}

	events::IEventDispatcher<InputDeviceEvent>& DeviceBase::getEventDispatcher() {
		return _eventDispatcher;
	}

	const InputDeviceInfo& DeviceBase::getInfo() const {
		return _info;
	}
}