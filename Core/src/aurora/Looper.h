#pragma once

#include "aurora/events/EventDispatcher.h"
#include <thread>

#if AE_OS == AE_OS_WIN
#include "mmsystem.h"
#pragma comment( lib, "Winmm.lib")
#endif

namespace aurora {
	enum class LooperEvent : uint8_t {
		TICKING,
		TICKED
	};


	class AE_DLL Looper : public Ref {
	public:
		Looper(f64 interval);
		virtual ~Looper();

		inline events::IEventDispatcher<LooperEvent>& AE_CALL getEventDispatcher() {
			return _eventDispatcher;
		}
		inline const events::IEventDispatcher<LooperEvent>& AE_CALL getEventDispatcher() const {
			return _eventDispatcher;
		}

		inline bool AE_CALL isRunning() const {
			return *_isRunning;
		}

		inline f64 AE_CALL getInterval() const {
			return _interval;
		}
		inline void AE_CALL setInterval(f64 interval) {
			_interval = interval < 0. ? 0. : interval;
		}

		inline void AE_CALL resetDeltaRecord() {
			_updatingCount = 0;
			_updateTimeCompensationFrameCount = 0;
		}

		void AE_CALL run(bool restriction);
		void AE_CALL tick(bool restriction);
		void AE_CALL stop();

	protected:
		events::EventDispatcher<LooperEvent> _eventDispatcher;

		std::shared_ptr<bool> _isRunning;
		f64 _interval;
		size_t _updatingCount;
		int64_t _updatingTimePoint;
		int64_t _updateTimeCompensationTimePoint;
		size_t _updateTimeCompensationFrameCount;

		inline void AE_CALL _sleep(size_t milliseconds) {
#if AE_OS == AE_OS_WIN
			timeBeginPeriod(1);
#endif
			std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
#if AE_OS == AE_OS_WIN
			timeEndPeriod(1);
#endif
		}
	};
}