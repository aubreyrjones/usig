//
// Created by ARJ on 11/22/15.
//

#include "ufrp4.h"
#include "../glm/glm/glm.hpp" // lots of glm's
#include "../glm/glm/gtx/string_cast.hpp"

#include <iostream>
#include <memory>

using namespace ufrp;
using namespace std;

struct Child {
	VarExpr<glm::vec2>::shared bounds {32.0f, 32.0f};
	VarExpr<int>::shared zDepth {12};
};

struct Parent {
	shared_value<glm::vec2> bounds;
	shared_value<int> zDepth;

	Parent(Child & c) {
		bounds = make_shared_expr(c.bounds + constant<glm::vec2>(2, 2));
		zDepth = make_shared_expr(c.zDepth + constant(6));
	}
};

int main() {
	Child c;
	Parent p(c);

	cout << glm::to_string((*p.bounds)()) << endl;
	cout << *p.zDepth << endl;

	return 0;
}