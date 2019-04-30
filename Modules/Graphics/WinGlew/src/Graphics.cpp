#include "Graphics.h"
#include "base/Application.h"
#include "base/String.h"
#include "CreateModule.h"

#include "BaseTexture.h"

namespace aurora::modules::graphics::win_glew {
	Graphics::Graphics(Application* app, IProgramSourceTranslator* trans) :
		_app(app),
		_trans(trans),
		_dc(nullptr),
		_rc(nullptr),
		_majorVer(0),
		_minorVer(0),
		_intVer(0) {
	}

	Graphics::~Graphics() {
		_release();
	}

	bool Graphics::createDevice(const GraphicsAdapter* adapter) {
		if (_dc || !_app.get()->Win_getHWND()) return false;

		_dc = GetDC(_app.get()->Win_getHWND());
		if (!_dc) return false;

		PIXELFORMATDESCRIPTOR pfd;
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags =
			PFD_DRAW_TO_WINDOW |            //��ʽ֧�ִ���
			PFD_SUPPORT_OPENGL |            //��ʽ����֧��OpenGL
			PFD_DOUBLEBUFFER;               //����֧��˫����
		pfd.iPixelType = PFD_TYPE_RGBA;     //����RGBA ��ʽ
		pfd.cColorBits = 24;                //ѡ��ɫ�����
		pfd.cRedBits = 0;                   //����RGBA
		pfd.cRedShift = 0;
		pfd.cGreenBits = 0;
		pfd.cGreenShift = 0;
		pfd.cBlueBits = 0;
		pfd.cBlueShift = 0;
		pfd.cAlphaBits = 0;
		pfd.cAlphaShift = 0;
		pfd.cAccumBits = 0;                 //���ۼӻ���
		pfd.cAccumRedBits = 0;              //���Ծۼ�λ
		pfd.cAccumGreenBits = 0;
		pfd.cAccumBlueBits = 0;
		pfd.cAccumAlphaBits = 0;
		pfd.cDepthBits = 32;                //32λZ-����(��Ȼ���)
		pfd.cStencilBits = 0;               //���ɰ建��
		pfd.cAuxBuffers = 0;                //�޸�������
		pfd.iLayerType = PFD_MAIN_PLANE;    //����ͼ��
		pfd.bReserved = 0;                  //Reserved
		pfd.dwLayerMask = 0;                //���Բ�����
		pfd.dwVisibleMask = 0;
		pfd.dwDamageMask = 0;

		auto pf = ChoosePixelFormat(_dc, &pfd);
		if (!SetPixelFormat(_dc, pf, &pfd)) {
			_release();
			return false;
		}

		_rc = wglCreateContext(_dc);
		if (!_rc) {
			_release();
			return false;
		}

		wglMakeCurrent(_dc, _rc);
		if (glewInit() != GLEW_OK) {
			_release();
			return false;
		}

		glFrontFace(GL_CW);

		glGetIntegerv(GL_MAJOR_VERSION, &_majorVer);
		glGetIntegerv(GL_MINOR_VERSION, &_minorVer);

		_intVer = _majorVer * 100 + _minorVer * 10;
		_strVer = String::toString(_intVer);

		auto tex = new BaseTexture(TextureType::TEX2D);
		tex->create(this, Vec3ui32(), 1, 1, TextureFormat::R8G8B8, Usage::NONE, nullptr);

		return true;
	}

	IConstantBuffer* Graphics::createConstantBuffer() {
		return nullptr;
	}

	IIndexBuffer* Graphics::createIndexBuffer() {
		return new IndexBuffer(*this);
	}

	IProgram* Graphics::createProgram() {
		return new Program(*this);
	}

	ISampler* Graphics::createSampler() {
		return nullptr;
	}

	ITexture1DResource* Graphics::createTexture1DResource() {
		return nullptr;
	}

	ITexture2DResource* Graphics::createTexture2DResource() {
		return nullptr;
	}

	ITexture3DResource* Graphics::createTexture3DResource() {
		return nullptr;
	}

	ITextureView* Graphics::createTextureView() {
		return nullptr;
	}

	IVertexBuffer* Graphics::createVertexBuffer() {
		return new VertexBuffer(*this);
	}

	void Graphics::beginRender() {
		wglMakeCurrent(_dc, _rc);

		i32 w, h;
		_app.get()->getInnerSize(w, h);
		glViewport(0, 0, w, h);
	}

	void Graphics::endRender() {
		//������ǰ�������ͺ�̨������
		SwapBuffers(_dc);

		if (wglGetCurrentContext() == _rc) wglMakeCurrent(nullptr, nullptr);
	}

	void Graphics::present() {
	}

	void Graphics::clear() {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Graphics::_release() {
		if (wglGetCurrentContext() == _rc) wglMakeCurrent(nullptr, nullptr);

		if (_rc) {
			wglDeleteContext(_rc);
			_rc = nullptr;
		}

		if (_dc) {
			ReleaseDC(_app.get()->Win_getHWND(), _dc);
			_dc = nullptr;
		}
	}

	GLenum Graphics::convertInternalFormat(TextureFormat fmt) {
		switch (fmt) {
		case TextureFormat::R8G8B8:
			return GL_RGB8;
		case TextureFormat::R8G8B8A8:
			return GL_RGBA8;
		default:
			return 0;
		}
	}
}