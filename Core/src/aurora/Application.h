#pragma once

#include "aurora/Ref.h"
#include "aurora/math/Box.h"
#include "aurora/events/EventDispatcher.h"

#if AE_OS == AE_OS_WIN
#include "mmsystem.h"
#pragma comment( lib, "Winmm.lib")
#endif

namespace aurora {
	class IClocker;


	enum class ApplicationEvent : uint8_t {
		RESIZED,
		FOCUS_IN,
		FOCUS_OUT,
		CLOSING,
		CLOSED
	};


	class AE_DLL Application : public Ref {
	public:
		class AE_DLL Style {
		public:
			bool maximizeButton = false;
			bool minimizeButton = true;
			bool thickFrame = false;
		};

		Application(const std::string_view& appId);
		virtual ~Application();

		inline events::IEventDispatcher<ApplicationEvent>& AE_CALL getEventDispatcher() {
			return _eventDispatcher;
		}
		inline const events::IEventDispatcher<ApplicationEvent>& AE_CALL getEventDispatcher() const {
			return _eventDispatcher;
		}

		bool AE_CALL createWindow(const Style& style, const std::string_view& title, const Box2i32& windowedRect, bool fullscreen);
		bool AE_CALL isWindowed() const;
		void AE_CALL toggleFullscreen();
		inline Vec2i32 AE_CALL getInnerSize() const {
			Vec2i32 size(NO_INIT);
			getInnerSize(size);
			return size;
		}
		void AE_CALL getInnerSize(Vec2i32& size) const;
		void AE_CALL getWindowedRect(Box2i32& dst) const;
		void AE_CALL setWindowedRect(const Box2i32& rect);
		void AE_CALL setWindowTitle(const std::string_view& title);
		void AE_CALL setCursorVisible(bool isVisible);
		bool AE_CALL hasFocus() const;
		void AE_CALL pollEvents();

		bool AE_CALL isVisible() const;
		void AE_CALL setVisible(bool b);
		void AE_CALL shutdown();

		inline const std::string& getAppId() const {
			return _appId;
		}

		const std::string& getAppPath() const;

#if AE_OS == AE_OS_WIN
		inline HWND AE_CALL Win_getHWnd() const {
			return _hWnd;
		}
#endif

	protected:
		bool _isWindowed;
		bool _isClosing;
		Style _style;
		std::string _appId;

		events::EventDispatcher<ApplicationEvent> _eventDispatcher;

		mutable Box2i32 _windowedRect;
		Box2i32 _wndRect;
		
		mutable std::string _appPath;

		bool AE_CALL _adjustWindowRect(const Box2i32& in, Box2i32& out);
		void AE_CALL _recordWindowedRect() const;

#if AE_OS == AE_OS_WIN
		HINSTANCE _hIns;
		HWND _hWnd;

		RECT _lastWndInnerRect;

		DWORD AE_CALL _getWindowStyle() const;
		DWORD AE_CALL _getWindowExStyle() const;
		void AE_CALL _updateWindowRectValue();
		void AE_CALL _changeWindow(bool style, bool posOrSize);

		static LRESULT CALLBACK _wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
	};
}