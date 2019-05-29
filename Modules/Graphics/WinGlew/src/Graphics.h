#pragma once

#include "Base.h"
#include "modules/graphics/ConstantBufferManager.h"

namespace aurora::modules::graphics::win_glew {
	class AE_MODULE_DLL Graphics : public IGraphicsModule {
	public:
		struct InternalFeatures {
			bool supportTexStorage;
			GLfloat maxAnisotropy;
		};


		Graphics(Ref* loader, Application* app, IProgramSourceTranslator* trans);
		virtual ~Graphics();

		virtual const std::string& AE_CALL getVersion() const override;
		virtual const GraphicsDeviceFeatures& AE_CALL getDeviceFeatures() const override;
		virtual IConstantBuffer* AE_CALL createConstantBuffer() override;
		virtual IIndexBuffer* AE_CALL createIndexBuffer() override;
		virtual IProgram* AE_CALL createProgram() override;
		virtual ISampler* AE_CALL createSampler() override;
		virtual ITexture1DResource* AE_CALL createTexture1DResource() override;
		virtual ITexture2DResource* AE_CALL createTexture2DResource() override;
		virtual ITexture3DResource* AE_CALL createTexture3DResource() override;
		virtual ITextureView* AE_CALL createTextureView() override;
		virtual IVertexBuffer* AE_CALL createVertexBuffer() override;
		virtual IPixelBuffer* AE_CALL createPixelBuffer() override;
		
		virtual void AE_CALL beginRender() override;
		virtual void AE_CALL endRender() override;
		virtual void AE_CALL present() override;

		virtual void AE_CALL clear() override;

		bool AE_CALL createDevice(const GraphicsAdapter* adapter);

		inline IProgramSourceTranslator* AE_CALL getProgramSourceTranslator() const {
			return _trans.get();
		}

		inline ConstantBufferManager& AE_CALL getConstantBufferManager() {
			return _constantBufferManager;
		}

		inline bool AE_CALL isGreatThanVersion(GLint major, GLint minor) const {
			if (_majorVer > major) {
				return true;
			} else if (_majorVer < major) {
				return false;
			} else {
				return _minorVer >= minor;
			}
		}

		inline ui32 AE_CALL getIntVersion() const {
			return _intVer;
		}

		inline const std::string& AE_CALL getStringVersion() const {
			return _strVer;
		}

		inline const InternalFeatures& AE_CALL getInternalFeatures() const {
			return _internalFeatures;
		}

		inline const Usage AE_CALL getCreateBufferMask() const {
			return _createBufferMask;
		}

		static void AE_CALL convertFormat(TextureFormat fmt, GLenum& internalFormat, GLenum& format, GLenum& type);
		static ui32 AE_CALL getGLTypeSize(GLenum type);

	private:
		Usage _createBufferMask;

		RefPtr<Ref> _loader;
		RefPtr<Application> _app;
		RefPtr<IProgramSourceTranslator> _trans;

		InternalFeatures _internalFeatures;
		GraphicsDeviceFeatures _deviceFeatures;

		HDC _dc;
		HGLRC _rc;

		GLint _majorVer;
		GLint _minorVer;
		ui32 _intVer;
		std::string _strVer;
		inline static const std::string _moduleVersion = "0.1.0";
		std::string _deviceVersion;

		ConstantBufferManager _constantBufferManager;

		bool AE_CALL _glInit();
		void AE_CALL _release();

		IConstantBuffer* AE_CALL _createdShareConstantBuffer();
		IConstantBuffer* AE_CALL _createdExclusiveConstantBuffer(ui32 numParameters);

		static void GLAPIENTRY _debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
	};
}