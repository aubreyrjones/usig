//
// Created by ARJ on 11/22/15.
//

#ifndef USIG_UFRP4_H
#define USIG_UFRP4_H

namespace ufrp {

template <typename V, typename E>
struct Expr {
	typedef V value_type;

	value_type operator()() const { return static_cast<E const&>(*this)(); }

	//operator value_type() { return (*this)(); }
};

template <typename V, V _value>
struct ConstExpr : public Expr<V, ConstExpr<V, _value>> {
	typedef V value_type;

	V operator()() const { return _value; }
};


template <typename V1, typename E1>
struct NegateExpr : public Expr<V1, NegateExpr<V1, E1>> {
	typedef V1 value_type;

	V1 operator()() const { return -(_e1()); }

	NegateExpr(E1 const& e1) : _e1(e1) {}

private:
	E1 _e1;
};


template <typename E1, typename E2>
struct Sum : Expr<decltype((typename E1::value_type()) + (typename E2::value_type())), Sum<E1, E2>> {
	typedef decltype((typename E1::value_type()) + (typename E2::value_type())) value_type;

	value_type operator()() const { return _e1() + _e2(); }

	Sum(E1 const& e1, E2 const& e2) : _e1(e1), _e2(e2) {}

private:
	E1 _e1;
	E2 _e2;
};

template <typename E>
struct expr_for {
	typedef Expr<typename E::value_type, E> type;
};

template <typename E1>
typename expr_for<NegateExpr<typename E1::value_type, typename expr_for<E1>::type>>::type
operator-(E1 const& e1) {
	return NegateExpr<typename E1::value_type, typename expr_for<E1>::type>(e1);
};


template <typename E1, typename E2>

typename expr_for<Sum<typename expr_for<E1>::type, typename expr_for<E2>::type>>::type
operator+(E1 const& e1, E2 const& e2) {
	return Sum<typename expr_for<E1>::type, typename expr_for<E2>::type>(e1, e2);
};

}

#endif //USIG_UFRP4_H
