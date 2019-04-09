#pragma once

#include "Base.h"

namespace aurora::modules::graphics::win_glew {
	class Graphics;

	class AE_MODULE_DLL BaseBuffer {
	public:
		virtual ~BaseBuffer();

	protected:
		BaseBuffer(GLenum target);

		bool AE_CALL _stroage(ui32 size, const void* data = nullptr);
		void AE_CALL _write(ui32 offset, const void* data, ui32 length);
		void AE_CALL _flush();

	protected:
		bool _dirty;
		GLenum _target;
		ui32 _size;
		GLuint _handle;
		void* _mapData;

		GLsync _sync;

		void _delBuffer();
		void _waitServerSync();
		void _delSync();
	};
}