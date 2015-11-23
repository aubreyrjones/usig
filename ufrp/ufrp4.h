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

	virtual value_type value() { return V(); }

	value_type operator()() { return value(); }
	operator value_type() { return value(); }

	usig::signal<> s_Updated;
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

	PointerToExpr(std::shared_ptr<E> e) : _e(e) {}
	PointerToExpr() : _e() {}

	std::shared_ptr<E> get() { return _e; }

	operator E&() { return static_cast<E&>(*_e); }
	operator E const&() { return static_cast<E const&>(*_e); }

	PointerToExpr& operator=(typename E::value_type const& v) {
		*_e = v;
		return *this;
	}

private:
	std::shared_ptr<E> _e;
};


template <typename V>
struct VarExpr : public Expr<V, VarExpr<V>> {
	typedef V value_type;
	typedef PointerToExpr<VarExpr<V>> shared;

	value_type operator()() const { return _value; }

	VarExpr() : _value() {}
	VarExpr(value_type const& v) : _value(v) {  }

	VarExpr& operator=(value_type const& v) {
		_value = v;
		this->s_Updated();

		return *this;
	}

	template <class...Args>
	static shared make_shared(Args...args) {
		return PointerToExpr<VarExpr<V>>(std::make_shared<VarExpr<V>>(std::forward<Args>(args)...));
	}

private:
	value_type _value;
};


template <typename V, V _value>
struct ConstExpr : public Expr<V, ConstExpr<V, _value>> {
	typedef V value_type;

	value_type operator()() const { return _value; }
};


template <typename E1>
struct NegateExpr : public Expr<typename E1::value_type, NegateExpr<E1>> {
	typedef typename E1::value_type value_type;

	value_type operator()() const { return -(_e1()); }

	NegateExpr(E1 const& e1) : _e1(e1) {}

private:
	E1 _e1;
};



// =========== binary expressions ===========

template <typename R, typename E1, typename E2, typename E>
struct BinExpr {
	BinExpr(E1 const& e1, E2 const& e2) : _e1(e1), _e2(e2) {
	}

	BinExpr(BinExpr const& o) : BinExpr(o._e1, o._e2) {}

	operator E &() { return static_cast<E &>(*this); }
	operator E const&() { return static_cast<E const&>(*this); }

protected:
	E1 _e1;
	E2 _e2;
};


template <typename E1, typename E2>
struct Sum : public Expr<decltype(typename E1::value_type() + typename E2::value_type()), Sum<E1, E2>>, BinExpr<decltype(typename E1::value_type() + typename E2::value_type()), E1, E2, Sum<E1, E2>> {
	typedef decltype(typename E1::value_type() + typename E2::value_type()) value_type;

	value_type operator()() const {
		return this->_e1() + this->_e2();
	}

	using BinExpr<value_type, E1, E2, Sum<E1, E2>>::BinExpr;
};



// =========== operator functions ===========

template <typename E>
struct expr_for {
	typedef Expr<typename E::value_type, E> type;
};

template <typename E>
struct expr_ptr_for {
	typedef PointerToExpr<E> type;
};

template <typename E>
PointerToExpr<E> expr_ptr(std::shared_ptr<E> const& e) {
	return PointerToExpr<E>(e);
}

template <typename E>
std::shared_ptr<E> shared_expr(E const& e) {
	return std::shared_ptr<E>(new E(e));
}

template <typename V>
using shared_value = std::shared_ptr<Value<V>>;

template <typename E>
using SharedExpr = PointerToExpr<E>;

template <typename E1>
typename expr_for<NegateExpr<E1>>::type
operator-(E1 const& e1) {
	return NegateExpr<E1>(e1);
};

template <typename E1, typename E2>
Sum<E1, E2> operator+(E1 const& e1, E2 const& e2) {
	return Sum<E1, E2>(e1, e2);
};

}

#endif //USIG_UFRP4_H

























