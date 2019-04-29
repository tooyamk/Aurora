#include "Texture1DResource.h"
#include "Graphics.h"

namespace aurora::modules::graphics::win_d3d11 {
	Texture1DResource::Texture1DResource(Graphics& graphics) : ITexture1DResource(graphics),
		_baseTexRes(D3D11_BIND_SHADER_RESOURCE),
		_view(graphics, true) {
	}

	Texture1DResource::~Texture1DResource() {
		_baseTexRes.releaseTex(_graphics.get<Graphics>());
	}

	TextureType Texture1DResource::getType() const {
		return TextureType::TEX1D;
	}

	const void* Texture1DResource::getNativeView() const {
		return _view.getNativeView();
	}

	const void* Texture1DResource::getNativeResource() const {
		return &_baseTexRes;
	}

	ui32 Texture1DResource::getArraySize() const {
		return _baseTexRes.arraySize;
	}

	ui32 Texture1DResource::getMipLevels() const {
		return _baseTexRes.mipLevels;
	}

	bool Texture1DResource::create(ui32 width, ui32 arraySize, TextureFormat format, ui32 mipLevels, Usage resUsage, const void*const* data) {
		auto rst = _baseTexRes.create(_graphics.get<Graphics>(), TextureType::TEX1D, Size3<ui32>(width, 1, 1), arraySize, format, mipLevels, resUsage, data);
		_view.create(this, 0, -1, 0, _baseTexRes.arraySize);
		return rst;
	}

	Usage Texture1DResource::map(ui32 arraySlice, ui32 mipSlice, Usage expectMapUsage) {
		return _baseTexRes.map(_graphics.get<Graphics>(), arraySlice, mipSlice, expectMapUsage);
	}

	void Texture1DResource::unmap(ui32 arraySlice, ui32 mipSlice) {
		_baseTexRes.unmap(_graphics.get<Graphics>(), arraySlice, mipSlice);
	}

	i32 Texture1DResource::read(ui32 arraySlice, ui32 mipSlice, ui32 offset, void* dst, ui32 dstLen, i32 readLen) {
		return _baseTexRes.read(arraySlice, mipSlice, offset, dst, dstLen, readLen);
	}

	i32 Texture1DResource::write(ui32 arraySlice, ui32 mipSlice, ui32 offset, const void* data, ui32 length) {
		return _baseTexRes.write(arraySlice, mipSlice, offset, data, length);
	}

	bool Texture1DResource::write(ui32 arraySlice, ui32 mipSlice, ui32 left, ui32 right, const void* data) {
		D3D11_BOX box;
		box.front = 0;
		box.back = 1;
		box.top = 0;
		box.bottom = 1;
		box.left = left;
		box.right = right;

		return _baseTexRes.write(_graphics.get<Graphics>(), arraySlice, mipSlice, box, data);
	}
}