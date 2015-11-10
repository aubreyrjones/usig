//
// Created by ARJ on 11/9/15.
//

#include "ufrp.h"
#include <iostream>

using namespace ufrp;
using std::make_shared;

int main(int, char**) {
	auto x = makeval<int>(2);
	auto y = makeval<int>(3);
	auto z = makeval<int>(9);

	*z = *x * *y;

	auto xray = makeexpr<int>([](int a, int b, int c) -> int {return a * b * c; },
					  x, y, z);

	std::cout << *xray << std::endl;

	*x = 7;

	std::cout << *xray << std::endl;

	return 0;
}