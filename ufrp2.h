//
// Created by ARJ on 11/19/15.
//

#ifndef USIG_UFRP2_H
#define USIG_UFRP2_H

#include <memory>

namespace ufrp {

using std::shared_ptr;


template <typename VAL_T, typename E>
struct Expr : public std::enable_shared_from_this<Expr<VAL_T, E>> {
public:
	typedef VAL_T value_type;

	virtual ~Expr() {}

	VAL_T operator()() const { return static_cast<E const&>(*this)(); }

	operator VAL_T() { return static_cast<E&>(*this); };

	operator E&() { return static_cast<E&>(*this); }
	operator E const&() const { return static_cast<E const&>(*this); }

	virtual Expr<VAL_T, E>& operator=(VAL_T const& v) { return *this; }
};


template <typename VAL_T>
class Value : public Expr<VAL_T, Value<VAL_T>> {
public:

	Value(VAL_T const& initialValue) : value(initialValue) {}

	VAL_T operator()() const { return value; }

	operator VAL_T() const { return value; }

	Value<VAL_T>& operator=(VAL_T const& v) { value = v; return *this; }

protected:
	VAL_T value;
};

template <typename VAL_T>
shared_ptr<Expr<VAL_T, Value<VAL_T>>> makeval(VAL_T const& value) {
	return std::make_shared<Value<VAL_T>>(value);
}


template <typename V1, typename V2, typename E1, typename E2>
struct Difference : Expr<decltype(V1() - V2()), Difference<V1, V2, E1, E2>> {
public:
	typedef decltype(V1() - V2()) value_type;

	Difference(shared_ptr<Expr<V1, E1>> e1, shared_ptr<Expr<V2, E2>> e2) : e1(e1), e2(e2) {}

	value_type operator()() const { return (*e1) - (*e2); }

	operator value_type() const { return (*this)(); }

protected:
	shared_ptr<Expr<V1, E1>> e1;
	shared_ptr<Expr<V2, E2>> e2;
};

template <typename V1, typename V2, typename E1, typename E2>
struct Sum : Expr<decltype(V1() + V2()), Sum<V1, V2, E1, E2>> {
public:
	typedef decltype(V1() + V2()) value_type;

	Sum(shared_ptr<Expr<V1, E1>> e1, shared_ptr<Expr<V2, E2>> e2) : e1(e1), e2(e2) {}

	value_type operator()() const { return (*e1) + (*e2); }

	operator value_type() const { return (*this)(); }

protected:
	shared_ptr<Expr<V1, E1>> e1;
	shared_ptr<Expr<V2, E2>> e2;
};

template <typename V1, typename V2, typename E1, typename E2>
struct Product : Expr<decltype(V1() * V2()), Product<V1, V2, E1, E2>> {
public:
	typedef decltype(V1() * V2()) value_type;

	Product(shared_ptr<Expr<V1, E1>> e1, shared_ptr<Expr<V2, E2>> e2) : e1(e1), e2(e2) {}

	value_type operator()() const { return (*e1) * (*e2); }

	operator value_type() const { return (*this)(); }

protected:
	shared_ptr<Expr<V1, E1>> e1;
	shared_ptr<Expr<V2, E2>> e2;
};

template <typename V1, typename V2, typename E1, typename E2>
struct Quotient : Expr<decltype(V1() / V2()), Quotient<V1, V2, E1, E2>> {
public:
	typedef decltype(V1() / V2()) value_type;

	Quotient(shared_ptr<Expr<V1, E1>> e1, shared_ptr<Expr<V2, E2>> e2) : e1(e1), e2(e2) {}

	value_type operator()() const { return (*e1) / (*e2); }

	operator value_type() const { return (*this)(); }

protected:
	shared_ptr<Expr<V1, E1>> e1;
	shared_ptr<Expr<V2, E2>> e2;
};



template <typename V1, typename V2, typename E1, typename E2>
shared_ptr<Expr<typename Difference<V1, V2, E1, E2>::value_type, Difference<V1, V2, E1, E2>>> operator-(shared_ptr<Expr<V1, E1>> const& e1, shared_ptr<Expr<V2, E2>> const& e2) {
	return std::make_shared<Difference<V1, V2, E1, E2>>(e1, e2);
};

template <typename V1, typename V2, typename E1, typename E2>
shared_ptr<Expr<typename Sum<V1, V2, E1, E2>::value_type, Sum<V1, V2, E1, E2>>> operator+(shared_ptr<Expr<V1, E1>> const& e1, shared_ptr<Expr<V2, E2>> const& e2) {
	return std::make_shared<Sum<V1, V2, E1, E2>>(e1, e2);
};

template <typename V1, typename V2, typename E1, typename E2>
shared_ptr<Expr<typename Product<V1, V2, E1, E2>::value_type, Product<V1, V2, E1, E2>>> operator*(shared_ptr<Expr<V1, E1>> const& e1, shared_ptr<Expr<V2, E2>> const& e2) {
	return std::make_shared<Product<V1, V2, E1, E2>>(e1, e2);
};

template <typename V1, typename V2, typename E1, typename E2>
shared_ptr<Expr<typename Quotient<V1, V2, E1, E2>::value_type, Quotient<V1, V2, E1, E2>>> operator/(shared_ptr<Expr<V1, E1>> const& e1, shared_ptr<Expr<V2, E2>> const& e2) {
	return std::make_shared<Quotient<V1, V2, E1, E2>>(e1, e2);
};


}

#endif //USIG_UFRP2_H
