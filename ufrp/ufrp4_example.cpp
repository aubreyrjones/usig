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

};

struct A {
	shared_value<int> v;

	A(B & b) {
		v = shared_expr(b.a + b.b);
	}
};

int main() {
	B b;
	A a(b);

	cout << *a.v << endl;
}