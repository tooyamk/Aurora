#include "Application.h"
#include <thread>

AE_NS_BEGIN

Application::Application(f64 frameInterval) : EventDispatcher(this),
	_time(0) {
	setFrameInterval(frameInterval);
}

void Application::setFrameInterval(f64 frameInterval) {
	_frameInterval = frameInterval <= 0 ? 0 : frameInterval;
}

void Application::resetDeltaRecord() {
	_time = 0;
}

void Application::run() {
	MSG msg;
	memset(&msg, 0, sizeof(msg));

	while (msg.message != WM_QUIT) {
		if (PeekMessage(
			&msg,     // �洢��Ϣ�Ľṹ��ָ��
			nullptr,  // ������Ϣ���߳���Ϣ���ᱻ���� 
			0,        // ��Ϣ������Сֵ; Ϊ0ʱ�������п�����Ϣ
			0,        // ��Ϣ�������ֵ; Ϊ0ʱ�������п�����Ϣ
			PM_REMOVE // ָ����Ϣ��δ���; ��Ϣ�ڴ������Ӷ������Ƴ�
		)) {
			TranslateMessage(&msg); // �任�������Ϣ���ַ���Ϣ���ַ���Ϣ�����͵������̵߳���Ϣ����
			DispatchMessage(&msg);  // �ɷ���Ϣ�����ڹ���
		} else {
			//if (++aa == 120) {
				//PostQuitMessage(0);
			//}

			update(true);
		}
	}
}

void Application::update(bool autoSleep) {
	auto t0 = getTimeNow<std::chrono::microseconds, std::chrono::steady_clock>();
	f64 dt = _time == 0 ? 0 : (t0 - _time) * 0.001;//ms
	_time = t0;

	this->dispatchEvent(ApplicationEvent::ENTER_FRAME);

	if (autoSleep) {
		auto t1 = getTimeNow<std::chrono::microseconds, std::chrono::steady_clock>();

		f64 timePhase = f64(t1 - t0);
		if (timePhase < _frameInterval) std::this_thread::sleep_for(std::chrono::microseconds(i64(_frameInterval - timePhase)));
	}
}

void Application::shutdown() {
	PostQuitMessage(0);
}

AE_NS_END