#include <iostream>
#include "usig.h"

using namespace usig;

struct A {
	signal<int> s_Bar;

	void emitBar(int number) {
		s_Bar(number);
	}
};


class B {
	void _onBar(int a) {
		std::cout << "onBar: " << a << std::endl;
	}

	void _onZoom(int a) {

	}

public:
	slot<int> onBar;

	B() : onBar(std::bind(&B::_onBar, this, std::placeholders::_1)) { }
	B(B const& o) : onBar(o.onBar, std::bind(&B::_onBar, this, std::placeholders::_1)) {}
};

int main() {
	A a;
	B b;

	a.s_Bar.connect(b.onBar);

	a.emitBar(1);

	B b2(b);
	a.emitBar(2);

	return 0;
}