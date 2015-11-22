// 	The MIT License (MIT)
//
//	Copyright (c) 2015 Aubrey R. Jones
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.

#include <atomic>
#include <memory>
#include "usig.h"

#ifndef USIG_UFRP_H
#define USIG_UFRP_H

namespace ufrp {

template <class VAL_T>
struct Value : public std::enable_shared_from_this<Value<VAL_T>> {
protected:
	VAL_T _value {}; ///< The cached value.

public:
	Value() {}
	Value(VAL_T const& v) : _value(v) {}
	virtual ~Value() {}

	usig::signal<> s_Updated; ///< Emitted after the value is updated.

	void set(VAL_T const& newVal) {
		_value = newVal;
		s_Updated();
	}

	Value& operator=(VAL_T const& newVal) {
		set(newVal);
		return *this;
	}

	operator VAL_T() const { return _value; }
};

template <class V>
usig::signal<> & update_signal(V & v) {
	return v->s_Updated;
}

template <class VAL_T, class...Args>
std::shared_ptr<Value<VAL_T>> makeval(Args...args) {
	return std::make_shared<Value<VAL_T>>(std::forward<Args>(args)...);
}


template <class VAL_T>
VAL_T deref_value(std::shared_ptr<Value<VAL_T>> & v) {
	return *v;
}

template <class VAL_T, class...Args>
class Expr : public Value<VAL_T> {
protected:

	template<int ...>
	struct seq { };

	template<int N, int ...S>
	struct gens : gens<N-1, N-1, S...> { };

	template<int ...S>
	struct gens<0, S...> {
		typedef seq<S...> type;
	};

	template <int... S>
	VAL_T callUpdateFunction(seq<S...>) {
		return updateFunction(deref_value(std::get<S>(args))...);
	}

	void _onUpdated() {
		Value<VAL_T>::set(callUpdateFunction(typename gens<sizeof...(Args)>::type()));
	}


public:
	typedef std::function<VAL_T(Args...)> updateFunction_t;

	usig::slot<> onUpdated { USIG_MBIND(&Expr::_onUpdated) };

	std::tuple<std::shared_ptr<Value<Args>>...> args;
	updateFunction_t updateFunction;

	Expr() : args() {};
	Expr(updateFunction_t f, std::shared_ptr<Value<Args>>..._args) : args(_args...), updateFunction(f) {
		usig::connect(onUpdated, update_signal(_args)...);
		_onUpdated();
	};

	operator Value<VAL_T>& () { return *this; }
};


template <class VAL_T, typename MF, typename...Args>
std::shared_ptr<Value<VAL_T>> makeexpr(MF f, std::shared_ptr<Value<Args>>...args) {
	return std::make_shared<Expr<VAL_T, Args...>>(std::forward<MF>(f), std::forward<std::shared_ptr<Value<Args>>>(args)...);
};

}

#endif //USIG_UFRP_H
