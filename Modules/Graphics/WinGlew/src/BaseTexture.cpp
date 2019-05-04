#include "BaseTexture.h"
#include "Graphics.h"
#include "base/Image.h"
#include <algorithm>

namespace aurora::modules::graphics::win_glew {
	BaseTexture::BaseTexture(TextureType texType) :
		dirty(false),
		texType(texType),
		size(0),
		handle(0),
		mapData(nullptr),
		sync(nullptr),
		glTexInfo({ 0 }),
		arraySize(0),
		mipLevels(0),
		resUsage(Usage::NONE),
		mapUsage(Usage::NONE) {
	}

	BaseTexture::~BaseTexture() {
		releaseTex();
	}

	bool BaseTexture::create(Graphics* graphics, const Vec3ui32& size, ui32 arraySize, ui32 mipLevels,
		TextureFormat format, Usage resUsage, const void*const* data) {
		releaseTex();

		if (mipLevels == 0) {
			mipLevels = Image::calcMipLevels(size.getMax());
		} else if (mipLevels > 1) {
			auto maxLevels = Image::calcMipLevels(size.getMax());
			if (mipLevels > maxLevels) mipLevels = maxLevels;
		}

		texSize.set(size);

		glGenTextures(1, &handle);

		if (handle) {
			this->resUsage = resUsage & Usage::GPU_WRITE;
			//this->size = size;

			GLbitfield flags = GL_MAP_WRITE_BIT
				| GL_MAP_PERSISTENT_BIT //�ڱ�ӳ��״̬�²�ͬ��
				| GL_MAP_COHERENT_BIT;  //���ݶ�GPU�����ɼ�

			Graphics::convertFormat(format, glTexInfo.internalFormat, glTexInfo.format, glTexInfo.type);

			bool hasTexStorage = graphics->isGreatThanVersion(4, 2);

			switch (texType) {
			case TextureType::TEX1D:
			{
				if (arraySize) {
					glTexInfo.target = GL_TEXTURE_1D_ARRAY;
					glBindTexture(glTexInfo.target, handle);
					if (hasTexStorage) {
						glTexStorage2D(glTexInfo.target, mipLevels, glTexInfo.internalFormat, size[0], arraySize);
						if (data) {
							auto w = size[0];
							for (ui32 i = 0; i < mipLevels; ++i) {
								for (ui32 j = 0; j < arraySize; ++j) glTexSubImage2D(glTexInfo.target, i, 0, 0, w, j, glTexInfo.format, glTexInfo.type, data[i + j * mipLevels]);
								w = Image::calcNextMipPixelSize(w);
							}
						}
					} else {
						auto w = size[0];
						for (ui32 i = 0; i < mipLevels; ++i) {
							for (ui32 j = 0; j < arraySize; ++j) glTexImage2D(glTexInfo.target, i, glTexInfo.internalFormat, w, j, 0, glTexInfo.format, glTexInfo.type, data[i + j * mipLevels]);
							w = Image::calcNextMipPixelSize(w);
						}
					}
				} else {
					glTexInfo.target = GL_TEXTURE_1D;
					glBindTexture(glTexInfo.target, handle);
					if (hasTexStorage) {
						glTexStorage1D(glTexInfo.target, mipLevels, glTexInfo.internalFormat, size[0]);
						if (data) {
							auto w = size[0];
							for (ui32 i = 0; i < mipLevels; ++i) {
								glTexSubImage1D(glTexInfo.target, i, 0, w, glTexInfo.format, glTexInfo.type, data[i]);
								w = Image::calcNextMipPixelSize(w);
							}
						}
					} else {
						auto w = size[0];
						for (ui32 i = 0; i < mipLevels; ++i) {
							glTexImage1D(glTexInfo.target, i, glTexInfo.internalFormat, w, 0, glTexInfo.format, glTexInfo.type, data[i]);
							w = Image::calcNextMipPixelSize(w);
						}
					}
				}

				break;
			}
			case TextureType::TEX2D:
			{
				if (arraySize) {
					glTexInfo.target = GL_TEXTURE_2D_ARRAY;
					glBindTexture(glTexInfo.target, handle);
					if (hasTexStorage) {
						glTexStorage3D(glTexInfo.target, mipLevels, glTexInfo.internalFormat, size[0], size[1], arraySize);
						if (data) {
							Vec2ui32 size2((ui32(&)[2])size);
							for (ui32 i = 0; i < mipLevels; ++i) {
								for (ui32 j = 0; j < arraySize; ++j) glTexSubImage3D(glTexInfo.target, i, 0, 0, 0, size2[0], size2[1], j, glTexInfo.format, glTexInfo.type, data[i + j * mipLevels]);
								Image::calcNextMipPixelSize(size2);
							}
						}
					} else {
						Vec2ui32 size2((ui32(&)[2])size);
						for (ui32 i = 0; i < mipLevels; ++i) {
							for (ui32 j = 0; j < arraySize; ++j) glTexImage3D(glTexInfo.target, i, glTexInfo.internalFormat, size2[0], size2[1], j, 0, glTexInfo.format, glTexInfo.type, data[i + j * mipLevels]);
							Image::calcNextMipPixelSize(size2);
						}
					}
				} else {
					glTexInfo.target = GL_TEXTURE_2D;
					glBindTexture(glTexInfo.target, handle);
					if (hasTexStorage) {
						glTexStorage2D(glTexInfo.target, mipLevels, glTexInfo.internalFormat, size[0], size[1]);
						if (data) {
							Vec2ui32 size2((ui32(&)[2])size);
							for (ui32 i = 0; i < mipLevels; ++i) {
								glTexSubImage2D(glTexInfo.target, i, 0, 0, size2[0], size2[1], glTexInfo.format, glTexInfo.type, data[i]);
								Image::calcNextMipPixelSize(size2);
							}
						}
					} else {
						Vec2ui32 size2((ui32(&)[2])size);
						for (ui32 i = 0; i < mipLevels; ++i) {
							glTexImage2D(glTexInfo.target, i, glTexInfo.internalFormat, size2[0], size2[1], 0, glTexInfo.format, glTexInfo.type, data[i]);
							Image::calcNextMipPixelSize(size2);
						}
					}
				}

				break;
			}
			case TextureType::TEX3D:
			{
				arraySize = 0;
				glTexInfo.target = GL_TEXTURE_3D;
				glBindTexture(glTexInfo.target, handle);
				if (hasTexStorage) {
					glTexStorage3D(glTexInfo.target, mipLevels, glTexInfo.internalFormat, size[0], size[1], size[2]);
					if (data) {
						auto size3 = size;
						for (ui32 i = 0; i < mipLevels; ++i) {
							glTexSubImage3D(glTexInfo.target, i, 0, 0, 0, size3[0], size3[1], size3[2], glTexInfo.format, glTexInfo.type, data[i]);
							Image::calcNextMipPixelSize(size3);
						}
					}
				} else {
					auto size3 = size;
					for (ui32 i = 0; i < mipLevels; ++i) {
						glTexImage3D(glTexInfo.target, i, glTexInfo.internalFormat, size3[0], size3[1], size3[2], 0, glTexInfo.format, glTexInfo.type, data[i]);
						Image::calcNextMipPixelSize(size3);
					}
				}

				break;
			}
			default:
				break;
			}

			this->arraySize = arraySize;
			this->mipLevels = mipLevels;
			//mapData = glMapBufferRange(texType, 0, size, flags);

			return true;
		}

		releaseTex();
		return false;
	}

	Usage BaseTexture::map(ui32 arraySlice, ui32 mipSlice, Usage expectMapUsage) {
		return Usage::NONE;
		/*
		Usage ret = Usage::NONE;

		expectMapUsage &= Usage::CPU_READ_WRITE;
		if (handle && expectMapUsage != Usage::NONE) {
			if (((expectMapUsage & Usage::CPU_READ) == Usage::CPU_READ) && ((resUsage & Usage::CPU_READ) == Usage::CPU_READ)) {
				ret |= Usage::CPU_READ;
			} else {
				expectMapUsage &= ~Usage::CPU_READ;
			}
			if ((expectMapUsage & Usage::CPU_WRITE) == Usage::CPU_WRITE) {
				if ((resUsage & Usage::CPU_WRITE) == Usage::CPU_WRITE) {
					ret |= Usage::CPU_WRITE | Usage::CPU_WRITE_NO_OVERWRITE;
				}
			} else {
				expectMapUsage &= ~Usage::CPU_WRITE;
			}

			mapUsage = expectMapUsage;
		}

		return ret;
		*/
	}

	void BaseTexture::unmap(ui32 arraySlice, ui32 mipSlice) {
		//mapUsage = Usage::NONE;
	}

	i32 BaseTexture::read(ui32 arraySlice, ui32 mipSlice, ui32 offset, void* dst, ui32 dstLen, i32 readLen) {
		return -1;
	}

	i32 BaseTexture::write(ui32 arraySlice, ui32 mipSlice, ui32 offset, const void* data, ui32 length) {
		/*
		if ((mapUsage & Usage::CPU_WRITE) == Usage::CPU_WRITE) {
			if (data && length && offset < size) {
				length = std::min<ui32>(length, size - offset);
				memcpy((i8*)mapData + offset, data, length);
				return length;
			}
			return 0;
		}
		*/
		return -1;
	}

	bool BaseTexture::update(ui32 arraySlice, ui32 mipSlice, const Box3ui32& range, const void* data) {
		if (handle && (resUsage & Usage::GPU_WRITE) == Usage::GPU_WRITE && arraySlice < arraySize && mipSlice < mipLevels) {
			glBindTexture(glTexInfo.internalFormat, handle);
			switch (glTexInfo.target) {
			case GL_TEXTURE_1D:
				glTexSubImage1D(glTexInfo.target, mipSlice, range.pos[0], range.size[0], glTexInfo.format, glTexInfo.type, data);
				return true;
			case GL_TEXTURE_1D_ARRAY:
				glTexSubImage2D(glTexInfo.target, mipSlice, range.pos[0], 0, range.size[0], arraySlice, glTexInfo.format, glTexInfo.type, data);
				return true;
			case GL_TEXTURE_2D:
				glTexSubImage2D(glTexInfo.target, mipSlice, range.pos[0], range.pos[1], range.size[0], range.size[1], glTexInfo.format, glTexInfo.type, data);
				return true;
			case GL_TEXTURE_2D_ARRAY:
				glTexSubImage3D(glTexInfo.target, mipSlice, range.pos[0], range.pos[1], 0, range.size[0], range.size[1], arraySlice, glTexInfo.format, glTexInfo.type, data);
				return true;
			case GL_TEXTURE_3D:
				glTexSubImage3D(glTexInfo.target, mipSlice, range.pos[0], range.pos[1], range.pos[2], range.size[0], range.size[1], range.size[2], glTexInfo.format, glTexInfo.type, data);
				return true;
			}
		}
		return false;
	}

	void BaseTexture::flush() {
		if (dirty) {
			waitServerSync();
			sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
			dirty = false;
		}
	}

	void BaseTexture::releaseTex() {
		if (handle) {
			releaseSync();

			if (mapData) {
				//glBindBuffer(texType, handle);
				//glUnmapBuffer(texType);
				mapData = nullptr;
			}

			glDeleteTextures(1, &handle);
			handle = 0;

			dirty = false;
			memset(&glTexInfo, 0, sizeof(glTexInfo));
			arraySize = 0;
			mipLevels = 0;
			texSize.set(0);
		}

		size = 0;
		resUsage = Usage::NONE;
		mapUsage = Usage::NONE;
	}

	void BaseTexture::waitServerSync() {
		if (sync) {
			do {
				auto rst = glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
				if (rst == GL_ALREADY_SIGNALED || rst == GL_CONDITION_SATISFIED) {
					releaseSync();
					return;
				}

			} while (true);
		}
	}

	void BaseTexture::releaseSync() {
		if (sync) {
			glDeleteSync(sync);
			sync = nullptr;
		}
	}
}