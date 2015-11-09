//
// Created by ARJ on 11/8/15.
//

#ifndef USIG_USIG_H
#define USIG_USIG_H

#include <functional>
#include <memory>
#include <vector>
#include <algorithm>
#include <mutex>

namespace usig {


template <typename FUNC, FUNC MemFun> struct member_binder;

template <class CLASS, class R, typename... Params, R(CLASS::*MemFun)(Params...)>
struct member_binder<R(CLASS::*)(Params...), MemFun> {
	typedef R (CLASS::*MemFun_t)(Params...);

	member_binder(CLASS* obj) : obj(obj) {}

	R operator()(Params...args) {
		(obj->*MemFun)(std::forward<Params>(args)...);
	}

private:
	CLASS *obj;
};


typedef std::lock_guard<std::mutex> lockg;


template<class... Args>
class signal;


/** A slot is called when a signal is emitted. */
template <class... Args>
class slot {
public:
	typedef std::function<void(Args...)> slot_function_t;
	typedef signal<Args...> signal_t;
	friend class signal<Args...>;

protected:
	slot_function_t const _slot; ///< Action functor.
	std::vector<signal_t *> connected_signals; ///< Signal to which we've connected, if any.
	std::mutex mutable _mutex;

	void add_signal(signal_t & s) {
		lockg _lock(_mutex);

		connected_signals.push_back(&s);
	}

	void remove_signal(signal_t const& s) {
		lockg _lock(_mutex);

		auto it = std::find(connected_signals.begin(), connected_signals.end(), &s);
		if (it != connected_signals.end()) {
			connected_signals.erase(it);
		}
	}

public:

	/// Construct a slot with the given function.
	slot(slot_function_t slot_function) : _slot(slot_function) {}

	/// Make a copy of the slot, rebinding the function.
	slot(slot const& o, slot_function_t slot_function) : _slot(slot_function) {
		lockg _lock(o._mutex);
		for (signal_t *s : o.connected_signals) {
			s->connect(*this);
		}
	}

	slot(slot const&) = delete; ///< Cannot copy slot, must rebind.

	~slot() { disconnect(); }

	/// Call the slot action function.
	void operator()(Args... args) {
		lockg _lock(_mutex);
		_slot(std::forward<Args>(args)...);
	}

	/// Is this slot connected to the given signal?
	bool connected_to(signal_t const& s) {
		lockg _lock(_mutex);
		return connected_signals.end() == std::find(connected_signals.begin(), connected_signals.end(), &s);
	}

	/// Disconnect this slot from all signals.
	void disconnect() {
		for (signal_t *s : connected_signals) {
			s->do_disconnect(*this);
		}

		connected_signals.clear();
	}
};


template <typename H, H FP> class mslot;

template <typename Class, typename... Args, void (Class::*MemFunPtr) (Args...)>
class mslot<void (Class::*) (Args...), MemFunPtr> : public slot<Args...> {
public:
	mslot (Class* obj) : slot<Args...>(member_binder<decltype(MemFunPtr), MemFunPtr>(obj)) {}
};

/** A synchronous signal. Calls all connected slots.*/
template <class... Args>
class signal {
public:
	typedef slot<Args...> slot_t;
	friend class slot<Args...>;

protected:
	std::vector<slot_t*> slots; ///< all connected slots.
	std::mutex mutable _mutex;

	void do_disconnect(slot_t & s) {
		lockg _lock(_mutex);
		auto it = std::find(slots.begin(), slots.end(), &s);
		if (it != slots.end()){
			slots.erase(it);
		}
	}

	void do_connect(slot_t & s) {
		lockg _lock(_mutex);
		slots.push_back(&s);
	}

public:

	signal() {}
	signal(signal& o) = delete;

	~signal() {
		for (slot_t * s : slots) {
			if (s->connected_to(*this)) {
				s->remove_signal(*this);
			}
		}
	}

	/// Emit the signal.
	void operator()(Args... args) {
		std::unique_lock<std::mutex> _lock(_mutex);

		for (slot_t * s : slots) {
			(*s)(std::forward<Args>(args)...);
		}
	}

	/// Connect a slot.
	void connect(slot_t & s) {
		do_connect(s);
		s.add_signal(*this);
	}

	/// Disconnect a slot.
	void disconnect(slot_t & s) {
		if (s.connected_to(*this)) {
			s.remove_signal(*this);
		}

		do_disconnect(s);
	}
};

}

#endif //USIG_USIG_H
