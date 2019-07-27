#pragma once

#include "Base.h"

namespace aurora::modules::graphics::win_d3d11 {
	class Graphics;

	class AE_MODULE_DLL Sampler : public ISampler {
	public:
		Sampler(Graphics& graphics);
		virtual ~Sampler();

		virtual void AE_CALL setFilter(SamplerFilterOperation op, SamplerFilterMode min, SamplerFilterMode mag, SamplerFilterMode mipmap) override;
		virtual void AE_CALL setComparisonFunc(SamplerComparisonFunc func) override;
		virtual void AE_CALL setAddress(SamplerAddressMode u, SamplerAddressMode v, SamplerAddressMode w) override;
		virtual void AE_CALL setMipLOD(f32 min, f32 max) override;
		virtual void AE_CALL setMipLODBias(f32 bias) override;
		virtual void AE_CALL setMaxAnisotropy (uint32_t max) override;
		virtual void AE_CALL setBorderColor(const Vec4f32& color) override;

		inline ID3D11SamplerState* AE_CALL getInternalSampler() const {
			return _samplerState;
		}

		void AE_CALL update();

	protected:
		bool _dirty;
		SamplerFilter _filter;
		SamplerAddress _address;
		D3D11_SAMPLER_DESC _desc;
		ID3D11SamplerState* _samplerState;

		void AE_CALL _updateFilter();

		static D3D11_COMPARISON_FUNC AE_CALL _convertComparisonFunc(SamplerComparisonFunc func);
		static D3D11_TEXTURE_ADDRESS_MODE AE_CALL _convertAddressMode(SamplerAddressMode mode);
		void AE_CALL _updateAddress();

		void AE_CALL _releaseRes();
	};
}