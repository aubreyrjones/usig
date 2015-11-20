//
// Created by ARJ on 11/19/15.
//


#include <iostream>

#include "glm/glm/glm.hpp" // lots of glm's
#include "glm/glm/gtx/string_cast.hpp"

#include "ufrp2.h"
using namespace ufrp;

struct A {
	value<float> foo = makeval(2.3f);
	value<glm::vec3> bar = makeval(glm::vec3(1, 2, 3));
};


int main(int argc, char **argv){
	A a;

	auto baz = a.foo * a.bar;

	std::cout << glm::to_string(valof(baz)) << std::endl;
}