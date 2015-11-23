//
// Created by ARJ on 11/22/15.
//

#include "ufrp4.h"
#include <iostream>
#include <memory>

using namespace ufrp;
using namespace std;

struct A {


};

int main() {

	ConstExpr<int, 112> c;

	VarExpr<int> v2(2);
	VarExpr<int> v(1);
	VarExpr<int> v3(268);

	auto v4 = v;

	cout << v2 + v4 + v3 + c << endl;

}