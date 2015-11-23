//
// Created by ARJ on 11/22/15.
//

#include "ufrp4.h"
#include <iostream>
#include <memory>

using namespace ufrp;
using namespace std;

struct B {
	ConstExpr<int, 2> a {};
	ConstExpr<int, 5> b {};
	SharedExpr<VarExpr<int>> c { shared_expr<VarExpr<int>>(28) };
};

struct A {
	shared_value<int> v;

	A(B & b) {
		v = shared_expr(b.a + b.b + b.c);
	}
};

struct C {
	shared_value<int> v;

	C(A & a) {
		v = shared_expr(expr_ptr(a.v) + ConstExpr<int, 298>());
	}
};

int main() {
	B b;
	A a(b);
	C c(a);

	cout << *a.v << endl;

	cout << *c.v << endl;

	*b.c.get() = 408;

	cout << *c.v << endl;


	return 0;
}