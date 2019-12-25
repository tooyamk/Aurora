#include "Texture1DResource.h"
#include "Graphics.h"

namespace aurora::modules::graphics::win_d3d11 {
	Texture1DResource::Texture1DResource(Graphics& graphics) : ITexture1DResource(graphics),
		_baseTexRes(D3D11_BIND_SHADER_RESOURCE),
		_view(graphics, true) {
	}

	Texture1DResource::~Texture1DResource() {
		_baseTexRes.releaseTex(*_graphics.get<Graphics>());
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

	uint16_t Texture1DResource::getPerPixelByteSize() const {
		return _baseTexRes.perPixelSize;
	}

	uint32_t Texture1DResource::getArraySize() const {
		return _baseTexRes.arraySize;
	}

	uint32_t Texture1DResource::getMipLevels() const {
		return _baseTexRes.mipLevels;
	}

	bool Texture1DResource::create(uint32_t width, uint32_t arraySize, uint32_t mipLevels, TextureFormat format, Usage resUsage, const void*const* data) {
		auto rst = _baseTexRes.create(*_graphics.get<Graphics>(), TextureType::TEX1D, Vec3ui32(width, 1, 1), arraySize, mipLevels, format, resUsage, data);
		_view.create(this, 0, -1, 0, _baseTexRes.arraySize);
		return rst;
	}

	Usage Texture1DResource::getUsage() const {
		return _baseTexRes.resUsage;
	}

	Usage Texture1DResource::map(uint32_t arraySlice, uint32_t mipSlice, Usage expectMapUsage) {
		return _baseTexRes.map(*_graphics.get<Graphics>(), arraySlice, mipSlice, expectMapUsage);
	}

	void Texture1DResource::unmap(uint32_t arraySlice, uint32_t mipSlice) {
		_baseTexRes.unmap(*_graphics.get<Graphics>(), arraySlice, mipSlice);
	}

	uint32_t Texture1DResource::read(uint32_t arraySlice, uint32_t mipSlice, uint32_t offset, void* dst, uint32_t dstLen) {
		return _baseTexRes.read(arraySlice, mipSlice, offset, dst, dstLen);
	}

	uint32_t Texture1DResource::write(uint32_t arraySlice, uint32_t mipSlice, uint32_t offset, const void* data, uint32_t length) {
		return _baseTexRes.write(arraySlice, mipSlice, offset, data, length);
	}

	bool Texture1DResource::update(uint32_t arraySlice, uint32_t mipSlice, const Box1ui32& range, const void* data) {
		D3D11_BOX box;
		box.left = range.pos[0];
		box.right = range.pos[0] + range.size[0];
		box.top = 0;
		box.bottom = 1;
		box.front = 0;
		box.back = 1;

		return _baseTexRes.update(*_graphics.get<Graphics>(), arraySlice, mipSlice, box, data);
	}

	bool Texture1DResource::copyFrom(uint32_t arraySlice, uint32_t mipSlice, const Box1ui32& range, const IPixelBuffer* pixelBuffer) {
		return false;
	}
}