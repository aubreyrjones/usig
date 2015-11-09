#include <iostream>
#include "usig.h"

using namespace usig;

struct A {
	signal<int> s_Bar;

	void emitBar(int number) {
		s_Bar(number);
	}
};


struct B {
	void _onBar(int a) {
		std::cout << this << " onBar: " << a << std::endl;
	}

	void _onZoom(int a) {
		std::cout << this << " onZoom: " << a << std::endl;
	}

public:
	slot<int> onBar;
	//slot<int> oonZoom {member_binder<decltype(&B::_onZoom), &B::_onZoom>()};
	mslot<decltype(&B::_onZoom), &B::_onZoom> onZoom {this};

	B() : onBar(std::bind(&B::_onBar, this, std::placeholders::_1)) { }
	B(B const& o) : onBar(o.onBar, std::bind(&B::_onBar, this, std::placeholders::_1)) {}
};


int foobar(int a, int b) {
	return a + b;
}

int main() {
	A a;
	B b1;

	a.s_Bar.connect(b1.onBar);
	a.s_Bar.connect(b1.onZoom);

	B b2(b1);

	a.emitBar(1);

	return 0;
}