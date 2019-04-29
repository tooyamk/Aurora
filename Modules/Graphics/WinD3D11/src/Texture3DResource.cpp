#include "Texture3DResource.h"
#include "Graphics.h"
#include "math/Box.h"
#include "math/Size3.h"

namespace aurora::modules::graphics::win_d3d11 {
	Texture3DResource::Texture3DResource(Graphics& graphics) : ITexture3DResource(graphics),
		_baseTexRes(D3D11_BIND_SHADER_RESOURCE),
		_view(graphics, true) {
	}

	Texture3DResource::~Texture3DResource() {
		_baseTexRes.releaseTex(_graphics.get<Graphics>());
	}

	TextureType Texture3DResource::getType() const {
		return TextureType::TEX3D;
	}

	const void* Texture3DResource::getNativeView() const {
		return _view.getNativeView();
	}

	const void* Texture3DResource::getNativeResource() const {
		return &_baseTexRes;
	}

	ui32 Texture3DResource::getArraySize() const {
		return _baseTexRes.arraySize;
	}

	ui32 Texture3DResource::getMipLevels() const {
		return _baseTexRes.mipLevels;
	}

	bool Texture3DResource::create(const Size3<ui32>& size, ui32 arraySize, TextureFormat format, ui32 mipLevels, Usage resUsage, const void*const* data) {
		auto rst = _baseTexRes.create(_graphics.get<Graphics>(), TextureType::TEX3D, size, arraySize, format, mipLevels, resUsage, data);
		_view.create(this, 0, -1, 0, _baseTexRes.arraySize);
		return rst;
	}

	Usage Texture3DResource::map(ui32 arraySlice, ui32 mipSlice, Usage expectMapUsage) {
		return _baseTexRes.map(_graphics.get<Graphics>(), arraySlice, mipSlice, expectMapUsage);
	}

	void Texture3DResource::unmap(ui32 arraySlice, ui32 mipSlice) {
		_baseTexRes.unmap(_graphics.get<Graphics>(), arraySlice, mipSlice);
	}

	i32 Texture3DResource::read(ui32 arraySlice, ui32 mipSlice, ui32 offset, void* dst, ui32 dstLen, i32 readLen) {
		return _baseTexRes.read(arraySlice, mipSlice, offset, dst, dstLen, readLen);
	}

	i32 Texture3DResource::write(ui32 arraySlice, ui32 mipSlice, ui32 offset, const void* data, ui32 length) {
		return _baseTexRes.write(arraySlice, mipSlice, offset, data, length);
	}

	bool Texture3DResource::write(ui32 arraySlice, ui32 mipSlice, const Box<ui32>& range, const void* data) {
		D3D11_BOX box;
		box.front = range.front;
		box.back = range.front + range.size.depth;
		box.top = range.top;
		box.bottom = range.top + range.size.height;
		box.left = range.left;
		box.right = range.left + range.size.width;

		return _baseTexRes.write(_graphics.get<Graphics>(), arraySlice, mipSlice, box, data);
	}
}