#include "VertexBuffer.h"
#include "Graphics.h"

namespace aurora::modules::graphics::win::glew {
	VertexBuffer::VertexBuffer(Graphics* graphics) :
		_graphics(graphics),
		_size(0),
		_handle(0),
		_mapData(nullptr) {
	}

	VertexBuffer::~VertexBuffer() {
		_graphics->unref();
	}

	bool VertexBuffer::stroage(ui32 size, const void* data) {
		_handle = 0;
		glGenBuffers(1, &_handle);

		if (_handle) {
			_size = size;

			GLbitfield flags = GL_MAP_WRITE_BIT
				| GL_MAP_PERSISTENT_BIT //�ڱ�ӳ��״̬�²�ͬ��
				| GL_MAP_COHERENT_BIT;  //���ݶ�GPU�����ɼ�

			glBindBuffer(GL_ARRAY_BUFFER, _handle);
			glBufferStorage(GL_ARRAY_BUFFER, size, data, flags);
			_mapData = glMapBuffer(GL_ARRAY_BUFFER, flags);
		}

		return false;
	}
}