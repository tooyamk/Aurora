#pragma once

#include "BaseResource.h"

namespace aurora::modules::graphics::win_d3d11 {
	class BaseBuffer : public BaseResource {
	public:
		BaseBuffer(UINT bufferType);
		virtual ~BaseBuffer();

		bool AE_CALL allocate(Graphics* graphics, ui32 size, Usage resUsage, const void* data = nullptr, ui32 dataSize = 0);
		Usage AE_CALL map(Graphics* graphics, Usage expectMapUsage);
		void AE_CALL unmap(Graphics* graphics);
		i32 AE_CALL read(ui32 offset, void* dst, ui32 dstLen, i32 readLen = -1);
		i32 AE_CALL write(Graphics* graphics, ui32 offset, const void* data, ui32 length);

		Usage mapUsage;
		D3D11_MAPPED_SUBRESOURCE mappedRes;

		void releaseBuffer(Graphics* graphics);
	};
}