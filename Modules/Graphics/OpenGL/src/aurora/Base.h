#pragma once

#include "aurora/modules/graphics/IGraphicsModule.h"
#include "aurora/modules/graphics/IProgramSourceTranslator.h"
#include "aurora/IApplication.h"
#include "aurora/hash/xxHash.h"

#include "GL/glew.h"

#if AE_OS == AE_OS_WIN
#	include <GL/GL.h>
#	include "GL/wglew.h"
#elif AE_OS == AE_OS_LINUX
#	include <GL/glx.h>
#endif

#include <optional>

namespace aurora::modules::graphics::gl {
	struct InternalBlendFunc {
		union {
			uint64_t featureValue;

			struct {
				uint16_t srcColor;
				uint16_t dstColor;
				uint16_t srcAlpha;
				uint16_t dstAlpha;
			};
		};
	};


	struct InternalBlendOp {
		union {
			uint32_t featureValue;

			struct {
				uint16_t color;
				uint16_t alpha;
			};
		};
	};


	struct InternalBlendWriteMask {
		union {
			uint32_t featureValue;

			bool rgba[4];
		};
	};

	
	struct InternalRenderTargetBlendState {
		RenderTargetBlendState state;
		InternalBlendFunc internalFunc;
		InternalBlendOp internalOp;
		InternalBlendWriteMask internalWriteMask;
	};


	struct InternalRasterizerState {
		union {
			uint64_t featureValue;

			struct {
				bool cullEnabled;
				uint8_t reserved;
				uint16_t fillMode;
				uint16_t cullMode;
				uint16_t frontFace;
			};
		};
	};


	struct InternalDepthState {
		InternalDepthState() :
			featureValue(0) {
		}

		union {
			uint32_t featureValue;

			struct {
				bool enabled;
				bool writeable;
				uint16_t func;
			};
		};
	};


	struct InternalStencilFaceState {
		InternalStencilFaceState() :
			func(GL_NEVER) {
			mask.featureValue = 0;
			op.featureValue = 0;
		}

		uint16_t func;

		struct {
			union {
				uint16_t featureValue;

				uint8_t read;
				uint8_t write;
			};
		} mask;

		struct {
			union {
				uint64_t featureValue;

				struct {
					uint16_t fail;
					uint16_t depthFail;
					uint16_t pass;
					uint16_t reserved;
				};
			};
		} op;
	};


	struct InternalStencilState {
		bool enabled;
		
		struct {
			InternalStencilFaceState front;
			InternalStencilFaceState back;
		} face;
	};


	inline uint64_t calcHash(const void* data, size_t size) {
		return hash::xxHash::calc<64, std::endian::native>(data, size, 0);
	}

	template<typename T>
	inline uint64_t calcHash(const T& val) {
		return calcHash(&val, sizeof(T));
	}
}