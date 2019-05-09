#include "Graphics.h"
#include "CreateModule.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "Program.h"
#include "Texture2DResource.h"
#include "VertexBuffer.h"
#include "base/Application.h"
#include "base/String.h"
#include "base/Time.h"
#include "GL/wglew.h"

namespace aurora::modules::graphics::win_glew {
	Graphics::Graphics(Application* app, IProgramSourceTranslator* trans) :
		_app(app),
		_trans(trans),
		_dc(nullptr),
		_rc(nullptr),
		_majorVer(0),
		_minorVer(0),
		_intVer(0),
		_deviceFeatures({ 0 }) {
		_constantBufferManager.createShareConstantBufferCallback = std::bind(&Graphics::_createdShareConstantBuffer, this);
		_constantBufferManager.createExclusiveConstantBufferCallback = std::bind(&Graphics::_createdExclusiveConstantBuffer, this, std::placeholders::_1);
	}

	Graphics::~Graphics() {
		_release();
	}

	bool Graphics::createDevice(const GraphicsAdapter* adapter) {
		if (_dc || !_app.get()->Win_getHWnd()) return false;

		/*
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
		*/

		if (!_glInit()) {
			_release();
			return false;
		}

		_dc = GetDC(_app.get()->Win_getHWnd());
		if (!_dc) return false;

		i32 iAttribIList[] = {
			WGL_SUPPORT_OPENGL_ARB, 1,
			WGL_DRAW_TO_WINDOW_ARB, 1,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_STENCIL_BITS_ARB, 8,
			WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,//MSAA
			WGL_SAMPLES_ARB, 4,//4x AA
			//WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			0 };

		i32 nPixelFormat = -1;
		ui32 nPixCount = 0;

		wglChoosePixelFormatARB(_dc, iAttribIList, nullptr, 1, &nPixelFormat, &nPixCount);
		if (nPixelFormat == -1) {
			_release();
			return false;
		}

		PIXELFORMATDESCRIPTOR pfd;
		if (!SetPixelFormat(_dc, nPixelFormat, &pfd)) {
			_release();
			return false;
		}
		
		GLint attribs[] = {
			//WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			//WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,//����ģʽ����
			//WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,//���Ĺ��ܻ���
#ifdef AE_DEBUG
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
			0
		};

		_rc = wglCreateContextAttribsARB(_dc, nullptr, attribs);
		if (!_rc) {
			_release();
			return false;
		}

		wglMakeCurrent(_dc, _rc);

		glFrontFace(GL_CW);

		glGetIntegerv(GL_MAJOR_VERSION, &_majorVer);
		glGetIntegerv(GL_MINOR_VERSION, &_minorVer);

#ifdef AE_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(&Graphics::_debugCallback, NULL);
		glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif

		_intVer = _majorVer * 100 + _minorVer * 10;
		_strVer = String::toString(_intVer);
		_deviceVersion = "OpenGL " + String::toString(_majorVer) + "." + String::toString(_minorVer);

		_deviceFeatures.supportSampler = isGreatThanVersion(3, 3);
		_deviceFeatures.supportTextureView = false;
		_deviceFeatures.supportConstantBuffer = isGreatThanVersion(3, 1);
		_deviceFeatures.supportPersisientMap = isGreatThanVersion(4, 4);

		return true;
	}

	const std::string& Graphics::getVersion() const {
		return _deviceVersion;
	}

	const GraphicsDeviceFeatures& Graphics::getDeviceFeatures() const {
		return _deviceFeatures;
	}

	IConstantBuffer* Graphics::createConstantBuffer() {
		return _deviceFeatures.supportConstantBuffer ? new ConstantBuffer(*this) : nullptr;
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
		return new Texture2DResource(*this);
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

		Vec2i32 size;
		_app.get()->getInnerSize(size);
		glViewport(0, 0, size[0], size[1]);
	}

	void Graphics::endRender() {
		//������ǰ�������ͺ�̨������
		SwapBuffers(_dc);

		//if (wglGetCurrentContext() == _rc) wglMakeCurrent(nullptr, nullptr);
	}

	void Graphics::present() {
	}

	void Graphics::clear() {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	bool Graphics::_glInit() {
		bool initOk = false;

		auto hIns = GetModuleHandle(nullptr);
		std::wstring className = L"OpenGL Temp Window " + String::Utf8ToUnicode(String::toString(Time::now()));

		WNDCLASSEXW wnd;
		memset(&wnd, 0, sizeof(wnd));
		wnd.cbSize = sizeof(wnd);
		wnd.lpfnWndProc = [](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			return DefWindowProc(hWnd, msg, wParam, lParam);
		};
		wnd.hInstance = hIns;
		wnd.lpszClassName = className.c_str();

		RegisterClassExW(&wnd);

		auto hwnd = CreateWindowEx(0, className.c_str(), L"", 0, 0, 0, 40, 40, nullptr, nullptr, nullptr, nullptr);
		if (hwnd) {
			auto dc = GetDC(hwnd);
			if (dc) {
				PIXELFORMATDESCRIPTOR pfd;
				if (SetPixelFormat(dc, 1, &pfd)) {
					auto rc = wglCreateContext(dc);
					if (rc) {
						wglMakeCurrent(dc, rc);
						initOk = glewInit() == GLEW_OK;
						wglMakeCurrent(nullptr, nullptr);
						wglDeleteContext(rc);
					}
				}
				ReleaseDC(hwnd, dc);
			}
			DestroyWindow(hwnd);
		}

		UnregisterClass(className.c_str(), hIns);

		return initOk;
	}

	void Graphics::_release() {
		if (wglGetCurrentContext() == _rc) wglMakeCurrent(nullptr, nullptr);

		if (_rc) {
			wglDeleteContext(_rc);
			_rc = nullptr;
		}

		if (_dc) {
			ReleaseDC(_app.get()->Win_getHWnd(), _dc);
			_dc = nullptr;
		}

		memset(&_deviceFeatures, 0, sizeof(_deviceFeatures));
		_deviceVersion = "OpenGL Unknown";
	}

	void Graphics::convertFormat(TextureFormat fmt, GLenum& internalFormat, GLenum& format, GLenum& type) {
		switch (fmt) {
		case TextureFormat::R8G8B8:
		{
			internalFormat = GL_RGB8;
			format = GL_RGB;
			type = GL_UNSIGNED_BYTE;

			break;
		}
		case TextureFormat::R8G8B8A8:
		{
			internalFormat = GL_RGBA8;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;

			break;
		}
		default:
		{
			internalFormat = 0;
			format = 0;
			type = 0;

			break;
		}
		}
	}

	ui32 Graphics::getGLTypeSize(GLenum type) {
		switch (type) {
		case GL_BOOL:
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
			return 1;
		case GL_SHORT:
		case GL_BOOL_VEC2:
			return 2;
		case GL_BOOL_VEC3:
			return 3;
		case GL_FLOAT:
		case GL_INT:
		case GL_UNSIGNED_INT:
		case GL_BOOL_VEC4:
			return 4;
		case GL_DOUBLE:
		case GL_FLOAT_VEC2:
		case GL_INT_VEC2:
		case GL_UNSIGNED_INT_VEC2:
			return 8;
		case GL_FLOAT_VEC3:
		case GL_INT_VEC3:
		case GL_UNSIGNED_INT_VEC3:
			return 12;
		case GL_DOUBLE_VEC2:
		case GL_FLOAT_VEC4:
		case GL_INT_VEC4:
		case GL_UNSIGNED_INT_VEC4:
		case GL_FLOAT_MAT2:
			return 16;
		case GL_DOUBLE_VEC3:
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT3x2:
			return 24;
		case GL_DOUBLE_VEC4:
		case GL_FLOAT_MAT2x4:
		case GL_FLOAT_MAT4x2:
		case GL_DOUBLE_MAT2:
			return 32;
		case GL_FLOAT_MAT3:
			return 36;
		case GL_FLOAT_MAT3x4:
		case GL_FLOAT_MAT4x3:
		case GL_DOUBLE_MAT2x3:
		case GL_DOUBLE_MAT3x2:
			return 48;
		case GL_FLOAT_MAT4:
		case GL_DOUBLE_MAT2x4:
		case GL_DOUBLE_MAT4x2:
			return 64;
		case GL_DOUBLE_MAT3:
			return 72;
		case GL_DOUBLE_MAT3x4:
		case GL_DOUBLE_MAT4x3:
			return 96;
		case GL_DOUBLE_MAT4:
			return 128;
		default:
			return 0;
		}
	}

	IConstantBuffer* Graphics::_createdShareConstantBuffer() {
		return new ConstantBuffer(*this);
	}

	IConstantBuffer* Graphics::_createdExclusiveConstantBuffer(ui32 numParameters) {
		auto cb = new ConstantBuffer(*this);
		cb->recordUpdateIds = new ui32[numParameters];
		memset(cb->recordUpdateIds, 0, sizeof(ui32) * numParameters);
		return cb;
	}

	void Graphics::_debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
		println("gl message : ", message);
	}
}