//
// Created by ARJ on 11/19/15.
//

#ifndef USIG_UFRP3_H
#define USIG_UFRP3_H

#include <memory>
#include "usig.h"

namespace ufrp {

using std::shared_ptr;
using namespace usig;

template <typename VAL_T>
struct Expr {
public:
	typedef VAL_T value_type;

	virtual ~Expr() {}

	virtual value_type operator()() const = 0;
	operator value_type() { return (*this)(); };

	virtual Expr& operator=(value_type const& v) {
		//no set
		return *this;
	}

	signal<> s_Updated;
};

template <typename VAL_T>
class Value : public Expr<VAL_T> {
public:

	Value(VAL_T const& initialValue) : _value(initialValue) {}

	VAL_T operator()() const { return _value; }

	Expr<VAL_T>& operator=(VAL_T const& v) {
		_value = v;
		this->s_Updated();
		return *this;
	}

protected:
	VAL_T _value;
};


template <typename VAL_T>
using expr = shared_ptr<Expr<VAL_T>>;

template <typename VAL_T>
shared_ptr<Expr<VAL_T>> makeval(VAL_T const& value) {
	return std::make_shared<Value<VAL_T>>(value);
}


// ================= expressions =================
template <typename R, typename V1, typename V2>
struct BinaryExpr : public Expr<R> {

	BinaryExpr(expr<V1> const& e1, expr<V2> const& e2) :
			Expr<R>(),
			e1(e1),
			e2(e2),
			onPredUpdate([this] { this->s_Updated(); })
	{
		e1->s_Updated.connect(onPredUpdate);
		e2->s_Updated.connect(onPredUpdate);
	}

	operator R() const { return (*this)(); }

protected:
	expr<V1> e1;
	expr<V2> e2;

	slot<> onPredUpdate;
};


template <typename V1, typename V2>
struct Difference : public BinaryExpr<decltype(V1() - V2()), V1, V2> {
	typedef BinaryExpr<decltype(V1() - V2()), V1, V2> BinExprType;
	using BinExprType::BinExprType;

	typename BinExprType::value_type operator()() const { return (*this->e1)() - (*this->e2)(); }
};

template <typename V1, typename V2>
struct Sum : public BinaryExpr<decltype(V1() + V2()), V1, V2> {
	typedef BinaryExpr<decltype(V1() + V2()), V1, V2> BinExprType;
	using BinExprType::BinExprType;

	typename BinExprType::value_type operator()() const { return (*this->e1)() + (*this->e2)(); }
};

template <typename V1, typename V2>
struct Product : public BinaryExpr<decltype(V1() * V2()), V1, V2> {
	typedef BinaryExpr<decltype(V1() * V2()), V1, V2> BinExprType;
	using BinExprType::BinExprType;

	typename BinExprType::value_type operator()() const { return (*this->e1)() * (*this->e2)(); }
};

template <typename V1, typename V2>
struct Quotient : public BinaryExpr<decltype(V1() / V2()), V1, V2> {
	typedef BinaryExpr<decltype(V1() / V2()), V1, V2> BinExprType;
	using BinExprType::BinExprType;

	typename BinExprType::value_type operator()() const { return (*this->e1)() / (*this->e2)(); }
};


template <typename V1, typename V2>
shared_ptr<Expr<typename Difference<V1, V2>::value_type>>
operator-(shared_ptr<Expr<V1>> const& e1, shared_ptr<Expr<V2>> const& e2) {
	return std::make_shared<Difference<V1, V2>>(e1, e2);
};

template <typename V1, typename V2>
shared_ptr<Expr<typename Sum<V1, V2>::value_type>>
operator+(shared_ptr<Expr<V1>> const& e1, shared_ptr<Expr<V2>> const& e2) {
	return std::make_shared<Sum<V1, V2>>(e1, e2);
};

template <typename V1, typename V2>
shared_ptr<Expr<typename Product<V1, V2>::value_type>>
operator*(shared_ptr<Expr<V1>> const& e1, shared_ptr<Expr<V2>> const& e2) {
	return std::make_shared<Product<V1, V2>>(e1, e2);
};

template <typename V1, typename V2>
shared_ptr<Expr<typename Quotient<V1, V2>::value_type>>
operator/(shared_ptr<Expr<V1>> const& e1, shared_ptr<Expr<V2>> const& e2) {
	return std::make_shared<Quotient<V1, V2>>(e1, e2);
};


template <typename VAL_T>
VAL_T valof(shared_ptr<Expr<VAL_T>> const& e) {
	return (*e)();
};

}

#endif //USIG_UFRP3_H
