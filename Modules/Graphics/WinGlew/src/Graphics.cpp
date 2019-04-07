#include "Graphics.h"
#include "base/Application.h"
#include "base/String.h"
#include "CreateModule.h"

namespace aurora::modules::graphics::win_glew {
	Graphics::Graphics(Application* app, IProgramSourceTranslator* trans) :
		_app(app->ref<Application>()),
		_trans(trans ? trans->ref<IProgramSourceTranslator>() : nullptr),
		_dc(nullptr),
		_rc(nullptr),
		_majorVer(0),
		_minorVer(0) {
	}

	Graphics::~Graphics() {
		_release();
		Ref::setNull(_trans);
		Ref::setNull(_app);
	}

	bool Graphics::createDevice() {
		if (_dc) return false;

		HWND hWnd = _app->$Win$_getHWND();
		if (!hWnd) return false;

		_dc = GetDC(hWnd);
		if (_dc) {
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
			if (!SetPixelFormat(_dc, pf, &pfd)) return false;
			
			_rc = wglCreateContext(_dc);
			if (_rc) {
				wglMakeCurrent(_dc, _rc);
				if (glewInit() == GLEW_OK) {
					glFrontFace(GL_CW);

					glGetIntegerv(GL_MAJOR_VERSION, &_majorVer);
					glGetIntegerv(GL_MINOR_VERSION, &_minorVer);

					_strVer = String::toString(_majorVer * 100 + _minorVer * 10);

					return true;
				}
			}
		}

		_release();
		return false;

		/*
		long style = GetWindowLong(hWnd, GWL_STYLE);

		DEVMODE dmScreenSettings;	 // Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);	 // Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth =GetSystemMetrics(SM_CXSCREEN);	 // Selected Screen Width
		dmScreenSettings.dmPelsHeight =  GetSystemMetrics(SM_CYSCREEN);	 // Selected Screen Height
		dmScreenSettings.dmBitsPerPel = 32;	 // Selected Bits Per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		//if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL) {
			SetWindowLong(hWnd, GWL_STYLE, style&(~WS_OVERLAPPEDWINDOW));
		//}
		SetWindowPos(hWnd,
			HWND_NOTOPMOST,
			0,
			0,
			GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN),
			SWP_SHOWWINDOW);
			*/
	}

	IIndexBuffer* Graphics::createIndexBuffer() {
		return new IndexBuffer(*this);
	}

	IProgram* Graphics::createProgram() {
		return new Program(*this);
	}

	IVertexBuffer* Graphics::createVertexBuffer() {
		return new VertexBuffer(*this);
	}

	void Graphics::beginRender() {
		wglMakeCurrent(_dc, _rc);

		i32 w, h;
		_app->getInnerSize(w, h);
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
			ReleaseDC(_app->$Win$_getHWND(), _dc);
			_dc = nullptr;
		}
	}
}