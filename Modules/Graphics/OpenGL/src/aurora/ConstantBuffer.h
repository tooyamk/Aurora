#pragma once

#include "BaseBuffer.h"

namespace aurora::modules::graphics::gl {
	class AE_MODULE_DLL ConstantBuffer : public IConstantBuffer {
	public:
		ConstantBuffer(Graphics& graphics);
		virtual ~ConstantBuffer();

		uint32_t* recordUpdateIds;

		virtual bool AE_CALL isCreated() const override;
		virtual const void* AE_CALL getNative() const override;
		virtual bool AE_CALL create(size_t size, Usage bufferUsage, const void* data = nullptr, size_t dataSize = 0) override;
		virtual size_t AE_CALL getSize() const override;
		virtual Usage AE_CALL getUsage() const override;
		virtual Usage AE_CALL map(Usage expectMapUsage) override;
		virtual void AE_CALL unmap() override;
		virtual size_t AE_CALL read(size_t offset, void* dst, size_t dstLen) override;
		virtual size_t AE_CALL write(size_t offset, const void* data, size_t length) override;
		virtual size_t AE_CALL update(size_t offset, const void* data, size_t length) override;
		//virtual void AE_CALL flush() override;
		virtual bool AE_CALL isSyncing() const override;
		virtual void AE_CALL destroy() override;

		inline GLuint AE_CALL getInternalBuffer() const {
			return _baseBuffer.handle;
		}

	protected:
		BaseBuffer _baseBuffer;
	};
}