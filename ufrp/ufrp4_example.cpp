//
// Created by ARJ on 11/22/15.
//

#include "ufrp4.h"
#include <iostream>
#include <memory>

using namespace ufrp;
using namespace std;


int main() {
	ConstExpr<int, 6> c;
	ConstExpr<int, 28> o;
//
	auto e2 = -c;
	auto e3 = -e2 + e2 + o;

	cout << e3() << endl;

	auto c2 = expr_ptr<ConstExpr<int, 203>>();

	auto e4 = c2 + c + c2;

	cout << e4() << endl;
}