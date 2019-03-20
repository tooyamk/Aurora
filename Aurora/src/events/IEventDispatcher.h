#pragma once

#include "base/Ref.h"
#include <functional>

namespace aurora::event {
	template<typename EvtType>
	class AE_TEMPLATE_DLL Event {
	public:
		Event(const EvtType& type, void* data = nullptr) :
			_type(type),
			_data(data),
			_target(nullptr) {
		}

		Event(void* target, const EvtType& type, void* data = nullptr) :
			_type(type),
			_data(data),
			_target(target) {
		}

		Event(void* target, const Event<EvtType>& e) :
			_type(e.getType()),
			_data(e.getData()),
			_target(target) {
		}

		inline void* getTarget() const {
			return _target;
		}

		inline const EvtType& getType() const {
			return _type;
		}

		inline void* getData() const {
			return _data;
		}

	protected:
		EvtType _type;
		void* _data;
		void* _target;
	};


	template<typename EvtType>
	using EVT_FN = void(*)(Event<EvtType>&);

	template<typename EvtType, typename Class>
	using EVT_METHOD_FN = void(Class::*)(Event<EvtType>&);

	template<typename EvtType>
	using EVT_FUNC = std::function<void(Event<EvtType>&)>;


	template<typename EvtType>
	class AE_TEMPLATE_DLL IEventListener {
	public:
		virtual void onEvent(Event<EvtType>& e) = 0;
	};


	template<typename EvtType>
	class AE_TEMPLATE_DLL FnEventListener : public IEventListener<EvtType> {
	public:
		FnEventListener(EVT_FN<EvtType> fn) :
			_fn(fn) {
		}

		virtual void onEvent(Event<EvtType>& e) override {
			_fn(e);
		}
	private:
		EVT_FN<EvtType> _fn;
	};


	template<typename EvtType>
	class AE_TEMPLATE_DLL FuncEventListener : public IEventListener<EvtType> {
	public:
		FuncEventListener(const EVT_FUNC<EvtType>& fn) :
			_fn(fn) {
		}

		virtual void onEvent(Event<EvtType>& e) override {
			_fn(e);
		}
	private:
		EVT_FUNC<EvtType> _fn;
	};


	template<typename EvtType, typename Class>
	class AE_TEMPLATE_DLL MethodEventListener : public IEventListener<EvtType> {
	public:
		MethodEventListener(Class* target, EVT_METHOD_FN<EvtType, Class> method) :
			_target(target),
			_method(method) {
		}

		virtual void onEvent(Event<EvtType>& e) override {
			(_target*->_method)(e);
		}
	private:
		Class* _target;
		EVT_METHOD_FN<EvtType, Class> _method;
	};


	template<typename EvtType>
	class AE_TEMPLATE_DLL IEventDispatcher : public Ref {
	public:
		virtual void AE_CALL addEventListener(const EvtType& type, IEventListener<EvtType>& listener) = 0;
		virtual bool AE_CALL hasEventListener(const EvtType& type) const = 0;
		virtual bool AE_CALL hasEventListener(const EvtType& type, const IEventListener<EvtType>& listener) const = 0;
		virtual void AE_CALL removeEventListener(const EvtType& type, const IEventListener<EvtType>& listener) = 0;
		virtual void AE_CALL removeEventListeners(const EvtType& type) = 0;
		virtual void AE_CALL removeEventListeners() = 0;

		virtual void AE_CALL dispatchEvent(const Event<EvtType>& e) = 0;
		virtual void AE_CALL dispatchEvent(void* target, const Event<EvtType>& e) = 0;
		virtual void AE_CALL dispatchEvent(const EvtType& type, void* data = nullptr) = 0;
		virtual void AE_CALL dispatchEvent(void* target, const EvtType& type, void* data = nullptr) = 0;
	};
}