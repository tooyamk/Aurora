#pragma once

#include "BaseBuffer.h"

namespace aurora::modules::graphics::win_glew {
	class AE_MODULE_DLL VertexBuffer : public IVertexBuffer {
	public:
		VertexBuffer(Graphics& graphics);
		virtual ~VertexBuffer();

		virtual const void* AE_CALL getNativeBuffer() const override;
		virtual bool AE_CALL create (uint32_t size, Usage bufferUsage, const void* data = nullptr, uint32_t dataSize = 0) override;
		virtual uint32_t AE_CALL getSize() const override;
		virtual Usage AE_CALL getUsage() const override;
		virtual Usage AE_CALL map(Usage expectMapUsage) override;
		virtual void AE_CALL unmap() override;
		virtual uint32_t AE_CALL read (uint32_t offset, void* dst, uint32_t dstLen) override;
		virtual uint32_t AE_CALL write (uint32_t offset, const void* data, uint32_t length) override;
		virtual uint32_t AE_CALL update (uint32_t offset, const void* data, uint32_t length) override;
		virtual void AE_CALL getFormat(VertexSize* size, VertexType* type) const override;
		virtual void AE_CALL setFormat(VertexSize size, VertexType type) override;
		//virtual void AE_CALL flush() override;
		virtual bool AE_CALL isSyncing() const override;

		bool AE_CALL use(GLuint index);

	protected:
		VertexSize _vertSize;
		VertexType _vertType;

		GLint _vertexSize;
		bool _validVertexFormat;
		GLenum _vertexType;
		BaseBuffer _baseBuffer;
	};
}