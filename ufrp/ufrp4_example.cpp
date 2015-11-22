//
// Created by ARJ on 11/22/15.
//

#include "ufrp4.h"
#include <iostream>

using namespace ufrp;
using namespace std;


int main() {

	ConstExpr<int, 6> c;

	auto e2 = -c;

	ConstExpr<int, 28> o;

	cout << e2() << endl;

	auto e3 = e2 + o;
//
//	cout << e3() << endl;
}