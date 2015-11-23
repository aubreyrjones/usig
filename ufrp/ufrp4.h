//
// Created by ARJ on 11/22/15.
//

#ifndef USIG_UFRP4_H
#define USIG_UFRP4_H

#include <memory>
#include <usig.h>
#include <iostream>

namespace ufrp {

template <typename V>
struct Value {
	typedef V value_type;

	Value() : onPredUpdated(USIG_MBIND(&Value<V>::emitUpdate)) {}
	Value(Value const& o) : onPredUpdated(USIG_MBIND(&Value<V>::emitUpdate)) {}

	virtual value_type value() { return V(); }

	value_type operator()() { return value(); }
	operator value_type() { return value(); }

	void emitUpdate() { s_Updated(); }

	usig::signal<> s_Updated;
	usig::slot<> onPredUpdated;
};

template <typename V, typename E>
struct Expr : Value<V> {
	typedef V value_type;

	value_type value() override { return (*this)(); }

	value_type operator()() const { return static_cast<E const&>(*this)(); }

	operator value_type() { return (*this)(); }

	operator E &() { return static_cast<E &>(*this); }
	operator E const&() { return static_cast<E const&>(*this); }
};


// =========== unary expressions ===========

template <typename E>
struct PointerToExpr : public Expr<typename E::value_type, PointerToExpr<E>> {
	typedef typename E::value_type value_type;

	value_type operator()() const { return (*_e)(); }

	PointerToExpr(std::shared_ptr<E> e) : _e(e) {
		usig::connect(this->onPredUpdated, _e->s_Updated);
	}

	PointerToExpr(PointerToExpr const& o) : PointerToExpr(o._e) {}

	std::shared_ptr<E> get() { return _e; }

	PointerToExpr& operator=(typename E::value_type const& v) {
		*_e = v;
		return *this;
	}

protected:
	std::shared_ptr<E> _e;
};


template <typename V>
struct VarExpr : public Expr<V, VarExpr<V>> {
	typedef V value_type;


	value_type operator()() const { return _value; }

	template <typename...Args>
	VarExpr(Args...args) : _value(std::forward<Args>(args)...) {}

	VarExpr& operator=(value_type const& v) {
		_value = v;
		this->emitUpdate();

		return *this;
	}

	struct shared : public PointerToExpr<VarExpr<V>> {
		shared(V const& v) : PointerToExpr<VarExpr<V>>(std::make_shared<VarExpr<V>>(v)) { }

		template <typename...Args>
		shared(Args...args) : PointerToExpr<VarExpr<V>>(std::make_shared<VarExpr<V>>(std::forward<Args>(args)...)) { }

		shared& operator=(V const& v) {
			*this->_e = v;
			return *this;
		}
	};

private:
	value_type _value;
};


template <typename V>
struct ConstExpr : public Expr<V, ConstExpr<V>> {
	typedef V value_type;

	value_type operator()() const { return _value; }

	template <typename...Args>
	ConstExpr(Args...args) : _value(std::forward<Args>(args)...) {}

private:
	V const _value;
};


template <template <typename V> class OP, typename E>
struct UnaryOpExpr : public Expr<typename OP<typename E::value_type>::result_type, UnaryOpExpr<OP, E>> {
	typedef typename OP<typename E::value_type>::result_type value_type;

	value_type operator()() const {
		return OP<typename E::value_type>()(this->_e());
	}

	UnaryOpExpr(E const& e) : _e(e) {
		usig::connect(this->onPredUpdated, _e.s_Updated);
	}

	UnaryOpExpr(UnaryOpExpr const& o) : UnaryOpExpr(o._e) {}

private:
	E _e;
};


// =========== binary expressions ===========

template <template <typename V1, typename V2> class OP, typename E1, typename E2>
struct BinOpExpr : public Expr<typename OP<typename E1::value_type, typename E2::value_type>::result_type, BinOpExpr<OP, E1, E2>> {
	typedef typename OP<typename E1::value_type, typename E2::value_type>::result_type value_type;

	value_type operator()() const {
		return OP<typename E1::value_type, typename E2::value_type>()(this->_e1(), this->_e2());
	}

	BinOpExpr(E1 const& e1, E2 const& e2) : _e1(e1), _e2(e2) {
		usig::connect(this->onPredUpdated, _e1.s_Updated, _e2.s_Updated);
	}

	BinOpExpr(BinOpExpr const& o) : BinOpExpr(o._e1, o._e2) {}

private:
	E1 _e1;
	E2 _e2;
};

// =========== utility functions ==========
template <typename E>
struct expr_for {
	typedef Expr<typename E::value_type, E> type;
};


// =========== operator functions ===========
template <typename V>
struct negate {
	typedef decltype(-V()) result_type;

	result_type operator()(V const& v) {
		return -v;
	}
};

template <typename E>
UnaryOpExpr<negate, E>
operator-(E const& e) {
	return UnaryOpExpr<negate, E>(e);
};

// binary operators

template <typename V1, typename V2>
struct add {
	typedef decltype(V1() + V2()) result_type;

	result_type operator()(V1 const& v1, V2 const& v2) {
		return v1 + v2;
	}
};

template <typename V1, typename V2>
struct subtract {
	typedef decltype(V1() - V2()) result_type;

	result_type operator()(V1 const& v1, V2 const& v2) {
		return v1 - v2;
	}
};

template <typename V1, typename V2>
struct multiply {
	typedef decltype(V1() * V2()) result_type;

	result_type operator()(V1 const& v1, V2 const& v2) {
		return v1 * v2;
	}
};

template <typename V1, typename V2>
struct divide {
	typedef decltype(V1() / V2()) result_type;

	result_type operator()(V1 const& v1, V2 const& v2) {
		return v1 / v2;
	}
};

template <typename E1, typename E2>
BinOpExpr<add, E1, E2>
operator+(E1 const& e1, E2 const& e2) {
	return BinOpExpr<add, E1, E2>(e1, e2);
};

template <typename E1, typename E2>
BinOpExpr<subtract, E1, E2>
operator-(E1 const& e1, E2 const& e2) {
	return BinOpExpr<subtract, E1, E2>(e1, e2);
};

template <typename E1, typename E2>
BinOpExpr<multiply, E1, E2>
operator*(E1 const& e1, E2 const& e2) {
	return BinOpExpr<multiply, E1, E2>(e1, e2);
};

template <typename E1, typename E2>
BinOpExpr<divide, E1, E2>
operator/(E1 const& e1, E2 const& e2) {
	return BinOpExpr<divide, E1, E2>(e1, e2);
};


// ======== pointer functions =========
template <typename V>
using shared_value = std::shared_ptr<Value<V>>;

template <typename E>
PointerToExpr<E> expr_ptr(std::shared_ptr<E> const& e) {
	return PointerToExpr<E>(e);
}

template <typename E>
std::shared_ptr<E> make_shared_expr(E const& e) {
	return std::shared_ptr<E>(new E(e));
}

template <typename V>
ConstExpr<V> constant(V const& v) {
	return ConstExpr<V>(v);
}

template <typename V, typename...Args>
ConstExpr<V> constant(Args...args) {
	return ConstExpr<V>(std::forward<Args>(args)...);
};

}

#endif //USIG_UFRP4_H

























