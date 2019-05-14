#pragma once

#include "Base.h"

namespace aurora::modules::graphics::win_glew {
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
		virtual void AE_CALL setMaxAnisotropy(ui32 max) override;
		virtual void AE_CALL setBorderColor(const Vec4f32& color) override;

		inline GLuint AE_CALL getInternalSampler() const {
			return _handle;
		}

		void AE_CALL update();

	protected:
		struct DirtyFlag {
			static const ui8 FILTER = 1;
			static const ui8 ADDRESS = 1 << 1;
			static const ui8 BORDER_COLOR = 1 << 2;
			static const ui8 COMPARE_FUNC = 1 << 3;
			static const ui8 LOD = 1 << 4;
			static const ui8 LOD_BAIS = 1 << 5;
			static const ui8 MAX_ANISOTROPY = 1 << 6;
		};


		GLuint _handle;
		ui8 _dirty;
		SamplerFilter _filter;
		SamplerAddress _address;


		struct {
			GLenum compareMode;
			GLenum min;
			GLenum mag;
		} _internalFilter;


		struct {
			GLenum s;
			GLenum t;
			GLenum r;
		} _internalAddress;


		struct {
			GLfloat min;
			GLfloat max;
		} _internalLOD;


		GLenum _internalCompareFunc;
		GLfloat _internalLODBias;
		GLfloat _maxAnisotropy;
		Vec4f32 _internalBorderColor;

		void AE_CALL _updateFilter();

		static GLenum AE_CALL _convertComparisonFunc(SamplerComparisonFunc func);
		static GLenum AE_CALL _convertAddressMode(SamplerAddressMode mode);
		void AE_CALL _updateAddress();

		void AE_CALL _releaseRes();
	};
}