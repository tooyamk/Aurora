#include "IndexBuffer.h"
#include "Graphics.h"

namespace aurora::modules::graphics::win_glew {
	IndexBuffer::IndexBuffer(Graphics& graphics) : BaseBuffer(GL_ELEMENT_ARRAY_BUFFER), IIndexBuffer(graphics),
		_indexType(0),
		_numElements(0) {
	}

	IndexBuffer::~IndexBuffer() {
	}

	bool IndexBuffer::stroage(ui32 size, const void* data) {
		auto rst = _stroage(size, data);
		_calcNumElements();
		return rst;
	}

	void IndexBuffer::write(ui32 offset, const void* data, ui32 length) {
		_write(offset, data, length);
	}

	void IndexBuffer::flush() {
		_flush();
	}

	void IndexBuffer::setFormat(IndexType type) {
		switch (type) {
		case IndexType::UI8:
			_indexType = GL_UNSIGNED_BYTE;
			break;
		case IndexType::UI16:
			_indexType = GL_UNSIGNED_SHORT;
			break;
		case IndexType::UI32:
			_indexType = GL_UNSIGNED_INT;
			break;
		default:
			_indexType = 0;
			break;
		}

		_calcNumElements();
	}

	void IndexBuffer::draw(ui32 count, ui32 offset) {
		if (_numElements > 0 && offset < _numElements) {
			ui32 last = _numElements - offset;
			if (count > _numElements) count = _numElements;
			if (count > last) count = last;

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _handle);
			glDrawRangeElements(GL_TRIANGLES, offset, _numElements, count, _indexType, nullptr);
		}
	}

	void IndexBuffer::_calcNumElements() {
		if (_size && _indexType) {
			switch (_indexType) {
			case GL_UNSIGNED_BYTE:
				_numElements = _size;
				break;
			case GL_UNSIGNED_SHORT:
				_numElements = _size >> 1;
				break;
			case GL_UNSIGNED_INT:
				_numElements = _size >> 2;
				break;
			default:
				_numElements = 0;
				break;
			}
		} else {
			_numElements = 0;
		}
	}
}