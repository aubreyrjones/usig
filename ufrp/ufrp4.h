//
// Created by ARJ on 11/22/15.
//

#ifndef USIG_UFRP4_H
#define USIG_UFRP4_H

#include <memory>

namespace ufrp {

template <typename V, typename E>
struct Expr {
	typedef E expr_type;
	typedef V value_type;

	value_type operator()() const { return static_cast<E const&>(*this)(); }

	operator value_type() { return (*this)(); }
};

// =========== unary expressions ===========

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

template <typename E>
struct PointerToExpr : public Expr<typename E::value_type, PointerToExpr<E>> {
	typedef typename E::value_type value_type;

	value_type operator()() const { return (*_e)(); }

	PointerToExpr(std::shared_ptr<E> e) : _e(e) {}
private:
	std::shared_ptr<E> _e;
};

// =========== binary expressions ===========

template <typename E1, typename E2>
struct BinExpr {
	BinExpr(E1 const& e1, E2 const& e2) : _e1(e1), _e2(e2) {}
protected:
	E1 _e1;
	E2 _e2;
};


template <typename E1, typename E2>
struct Sum : public Expr<decltype(typename E1::value_type() + typename E2::value_type()), Sum<E1, E2>>, public BinExpr<E1, E2> {
	typedef decltype(typename E1::value_type() + typename E2::value_type()) value_type;

	value_type operator()() const { return this->_e1() + this->_e2(); }

	using BinExpr<E1, E2>::BinExpr;
};


// =========== operator functions ===========

template <typename E>
struct expr_for {
	typedef Expr<typename E::value_type, E> type;
};

template <typename E>
PointerToExpr<E> expr_ptr(E const& e) {
	return PointerToExpr<E>(std::make_shared<E>(e));
}

template <typename E>
PointerToExpr<E> expr_ptr() {
	return PointerToExpr<E>(std::make_shared<E>());
}



template <typename E1>
typename expr_for<NegateExpr<E1>>::type
operator-(E1 const& e1) {
	return NegateExpr<E1>(e1);
};


template <typename E1, typename E2>
typename expr_for<Sum<typename expr_for<E1>::type, typename expr_for<E2>::type>>::type
operator+(Expr<typename E1::value_type, E1> const& e1, Expr<typename E2::value_type, E2> const& e2) {
	return Sum<typename expr_for<E1>::type, typename expr_for<E2>::type>(e1, e2);
};

}

#endif //USIG_UFRP4_H

























