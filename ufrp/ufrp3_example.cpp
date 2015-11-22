//
// Created by ARJ on 11/19/15.
//

#include "ufrp3.h"
#include <iostream>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtx/string_cast.hpp"

using namespace ufrp;

struct A {
	expr<float> foo = makeval(2.3f);
	expr<glm::vec3> bar = makeval(glm::vec3(1, 2, 3));
};

struct B {
	expr<glm::vec3> scaled;

	B(A & a) : scaled(a.foo * a.bar) {
		scaled->s_Updated.connect(onScaledChanged);
	}

	void printScaled() {
		std::cout << glm::to_string(valof(scaled)) << std::endl;
	}

	usig::slot<> onScaledChanged { USIG_MBIND(&B::printScaled) };
};

int main(int argc, char** argv) {
	A a;
	B b(a);

	b.printScaled();

	*a.foo = 78.234f;
}


