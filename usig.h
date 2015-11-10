//  The MIT License (MIT)
//
//  Copyright (c) 2015 Aubrey R. Jones
//
//		Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//		to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//		copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//		The above copyright notice and this permission notice shall be included in all
//		copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//		AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.

#ifndef USIG_USIG_H
#define USIG_USIG_H

#include <functional>
#include <vector>
#include <algorithm>
#include <mutex>

namespace usig {

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
	std::vector<signal_t *> connected_signals {}; ///< Signal to which we've connected, if any.
	std::mutex mutable _mutex {};

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

	/// Construct a slot. The given function will be called when a connected signal is emitted.
	slot(slot_function_t slot_function) : _slot(slot_function) {}

	slot(slot const&) = delete; ///< Cannot copy slot, must rebind.

	virtual ~slot() { disconnect(); }

	/**
	 * Call the slot action function. This is synchronized by the slot's mutex, meaning that the action function
	 * will not be called simultaneously by multiple signals.
	 * */
	void operator()(Args... args) {
		lockg _lock(_mutex);
		_slot(std::forward<Args>(args)...);
	}

	/// Is this slot connected to the given signal?
	bool connected_to(signal_t const& s) {
		lockg _lock(_mutex);
		return connected_signals.end() != std::find(connected_signals.begin(), connected_signals.end(), &s);
	}

	/// Clone the connections from the other slot.
	template <class OSLOT>
	void clone_connections(OSLOT const& o) {
		lockg _lock(o._mutex);

		for (signal_t *s: o.connected_signals) {
			s->connect(*this);
		}
	}

	/// Disconnect this slot from all signals.
	void disconnect() {
		for (signal_t *s : connected_signals) {
			s->do_disconnect(*this);
		}

		connected_signals.clear();
	}
};

/** A synchronous signal. Calls all connected slots.*/
template <class... Args>
class signal {
public:
	typedef slot<Args...> slot_t;
	friend class slot<Args...>;

protected:
	std::vector<slot_t*> slots {}; ///< all connected slots.
	std::mutex mutable _mutex {};

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

	signal(signal const& o) {
		lockg _lock(o._mutex);
		for (slot_t *s : o.slots){
			connect(*s);
		}
	}

	signal(signal && o) : slots(std::move(o.slots)) {
		for (slot_t *s : slots) {
			s->remove_signal(o);
			s->add_signal(*this);
		}
	}

	virtual ~signal() {
		for (slot_t * s : slots) {
			s->remove_signal(*this);
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


namespace util {
template<typename FUNC, FUNC MemFun>
struct member_binder;

template<class CLASS, class R, typename... Params, R(CLASS::*MemFun)(Params...)>
struct member_binder<R(CLASS::*)(Params...), MemFun> {
	typedef R (CLASS::*MemFun_t)(Params...);

	member_binder(CLASS *obj) : obj(obj) { }

	R operator()(Params...args) {
		(obj->*MemFun)(std::forward<Params>(args)...);
	}

private:
	CLASS *obj;
};

#define USIG_MBIND(slotname, obj) ::usig::util::member_binder<decltype(slotname), slotname>(obj)
} //namespace util


//template <typename F, F func, class C>
//auto mbind(C * obj) -> decltype(util::member_binder<F, func>(obj)) {
//	return util::member_binder<F, func>(obj);
//};

} //namespace usig

#endif //USIG_USIG_H
