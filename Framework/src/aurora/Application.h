#pragma once

#include "aurora/IApplication.h"
#include "aurora/math/Box.h"

#if AE_OS == AE_OS_LINUX
#	include <X11/Xlib.h>
#	include <X11/Xatom.h>
#	include <X11/Xutil.h>
#endif

namespace aurora {
	class AE_FW_DLL Application : public IApplication {
	public:
		Application(const std::string_view& appId);
		Application(const std::u8string_view& appId) : Application((const std::string_view&)appId) {}

		virtual ~Application();

		virtual events::IEventDispatcher<ApplicationEvent>& AE_CALL getEventDispatcher() override;
		virtual const events::IEventDispatcher<ApplicationEvent>& AE_CALL getEventDispatcher() const override;

		virtual bool AE_CALL createWindow(const ApplicationStyle& style, const std::string_view& title, const Vec2ui32& clientSize, bool fullscreen) override;
		virtual void* AE_CALL getNative(ApplicationNative native) const override;
		virtual bool AE_CALL isFullscreen() const override;
		virtual void AE_CALL toggleFullscreen() override;
		virtual Vec4ui32 AE_CALL getBorder() const override;
		virtual Vec2ui32 AE_CALL getCurrentClientSize() const override;
		virtual Vec2ui32 AE_CALL getClientSize() const override;
		virtual void AE_CALL setClientSize(const Vec2ui32& size) override;
		virtual void AE_CALL setWindowTitle(const std::string_view& title) override;
		inline void AE_CALL setWindowTitle(const std::u8string_view& title) {
			setWindowTitle((const std::string_view&)title);
		}
		virtual void AE_CALL setWindowPosition(const Vec2i32& pos) override;
		virtual void AE_CALL setCursorVisible(bool visible) override;
		virtual bool AE_CALL hasFocus() const override;
		virtual void AE_CALL setFocus() override;
		virtual bool AE_CALL isMaximzed() const override;
		virtual void AE_CALL setMaximum() override;
		virtual bool AE_CALL isMinimzed() const override;
		virtual void AE_CALL pollEvents() override;
		virtual void AE_CALL setMinimum() override;
		virtual void AE_CALL setRestore() override;
		virtual bool AE_CALL isVisible() const override;
		virtual void AE_CALL setVisible(bool b) override;
		virtual void AE_CALL shutdown() override;

		virtual std::string_view AE_CALL getAppId() const override;
		virtual const std::filesystem::path& AE_CALL getAppPath() const override;

	protected:
		bool _isFullscreen;
		bool _isClosing;
		bool _isVisible;
		ApplicationStyle _style;
		std::string _appId;
		Vec2ui32 _clientSize;
		Vec4i32 _border;//left, right, top, bottom

		events::EventDispatcher<ApplicationEvent> _eventDispatcher;

		mutable std::filesystem::path _appPath;
#if AE_OS == AE_OS_WIN
		enum class WindowState : uint8_t {
			NORMAL,
			MAXIMUM,
			MINIMUM
		};


		struct {
			WindowState wndState = WindowState::NORMAL;
			WindowState prevWndState = WindowState::NORMAL;
			bool wndDirty = false;
			bool ignoreEvtSize = false;
			HINSTANCE ins = nullptr;
			HWND wnd = nullptr;
			Vec2i32 clinetPos;
			Vec2ui32 sentSize;
			HBRUSH bkBrush = nullptr;
		} _win;


		void AE_CALL _calcBorder();
		Box2i32 AE_CALL _calcWindowRect(bool fullscreen) const;
		void AE_CALL _sendResizedEvent();
		Box2i32 AE_CALL _calcWorkArea() const;
		bool AE_CALL _setWndState(WindowState state);
		void AE_CALL _updateWindowPlacement(UINT showCmd = (std::numeric_limits<UINT>::max)());
		static DWORD AE_CALL _getWindowStyle(const ApplicationStyle& style, bool fullscreen);
		static DWORD AE_CALL _getWindowExStyle(bool fullscreen);

		template<bool FS>
		inline static constexpr UINT AE_CALL _getWindowPosFlags() {
			UINT flags = SWP_NOACTIVATE;
			if constexpr (FS) {
				flags |= SWP_NOCOPYBITS;
			} else {
				flags |= SWP_NOOWNERZORDER | SWP_NOZORDER;
			}
			return flags;
		}

		inline UINT AE_CALL _getWindowPosFlags() {
			return _isFullscreen ? _getWindowPosFlags<true>() : _getWindowPosFlags<false>();
		}

		static LRESULT CALLBACK _wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#elif AE_OS == AE_OS_LINUX
		enum class WindowState : uint8_t {
			NORMAL,
			MAXIMUM,
			MINIMUM
		};


		struct MwmHints {
			uint64_t flags;
			uint64_t functions;
			uint64_t decorations;
			int64_t input_mode;
			uint64_t status;

			static const uint64_t MWM_HINTS_FUNCTIONS = 1L << 0;
			static const uint64_t MWM_HINTS_DECORATIONS = 1L << 1;

			static const uint64_t MWM_FUNC_ALL = 1L << 0;
			static const uint64_t MWM_FUNC_RESIZE = 1L << 1;
			static const uint64_t MWM_FUNC_MOVE = 1L << 2;
			static const uint64_t MWM_FUNC_MINIMIZE = 1L << 3;
			static const uint64_t MWM_FUNC_MAXIMIZE = 1L << 4;
			static const uint64_t MWM_FUNC_CLOSE = 1L << 5;

			static const uint64_t MWM_DECOR_ALL = 1L << 0;
			static const uint64_t MWM_DECOR_BORDER = 1L << 1;
			static const uint64_t MWM_DECOR_RESIZEH = 1L << 2;
			static const uint64_t MWM_DECOR_TITLE = 1L << 3;
			static const uint64_t MWM_DECOR_MENU = 1L << 4;
			static const uint64_t MWM_DECOR_MINIMIZE = 1L << 5;
			static const uint64_t MWM_DECOR_MAXIMIZE = 1L << 6;
		};


		struct {
			WindowState wndState = WindowState::NORMAL;
			WindowState prevWndState = WindowState::NORMAL;
			bool wndDirty = false;
			bool xMapped = false;
			bool xFullscreen = false;
			WindowState xWndState = WindowState::NORMAL;
			bool ignoreEvtPos = false;
			Display* dis = nullptr;
			int32_t screen = 0;
			Window root = 0;
			Window wnd = 0;
			Vec2i32 wndPos;
			uint32_t bgColor = 0;
			Vec2ui32 sentSize;

			bool waitFrameEXTENTS = false;
			bool waitVisibility = false;

			Atom MOTIF_WM_HINTS;
			Atom WM_STATE;
			Atom WM_DELETE_WINDOW;
			Atom WM_PROTOCOLS;
			Atom NET_WM_PING;
			Atom NET_WM_WINDOW_TYPE;
			Atom NET_WM_WINDOW_TYPE_NORMAL;
			Atom NET_WM_STATE;
			Atom NET_WM_STATE_FULLSCREEN;
			Atom NET_WM_STATE_MAXIMIZED_HORZ;
			Atom NET_WM_STATE_MAXIMIZED_VERT;
			Atom NET_REQUEST_FRAME_EXTENTS;
			Atom NET_FRAME_EXTENTS;
			Atom NET_WORKAREA;
			Atom NET_CURRENT_DESKTOP;
		} _linux;

		void AE_CALL _sendClientEventToWM(Atom msgType, int64_t a = 0, int64_t b = 0, int64_t c = 0, int64_t d = 0, int64_t e = 0);
		void AE_CALL _sendResizedEvent();
		void AE_CALL _waitEvent(bool& value);
		Box2i32 AE_CALL _calcWorkArea() const;
		bool AE_CALL _setWndState(WindowState state);
		size_t AE_CALL _getXWndProperty(Window wnd, Atom property, Atom type, uint8_t** value) const;
		bool AE_CALL _isMaximized() const;
		bool AE_CALL _isMinimized() const;
		int32_t AE_CALL _getXWndState() const;
		void AE_CALL _updateWindowPlacement();

		static Bool AE_CALL _eventPredicate(Display* display, XEvent* event, XPointer pointer);
		void AE_CALL _doEvent(XEvent& e);
#endif
	};
}