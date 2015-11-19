//
// Created by ARJ on 11/19/15.
//

#include "ufrp2.h"
#include <iostream>

using namespace ufrp;


int main(int argc, char **argv){

	auto a = makeval(6);
	auto b = makeval(3.0);

	auto diff = a - b;

	std::cout << (*a) << std::endl;
	std::cout << (*diff) << std::endl;

	auto sum = diff + makeval(28);

	std::cout << (*sum) << std::endl;

	*a = 12;

	std::cout << (*sum) << std::endl;
}