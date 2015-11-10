#include <iostream>
#include "usig.h"

#include <thread>

using namespace usig;


namespace enc_ns {

struct B {
	void _onFoo(int a) {
		std::cout << this << " onFoo: " << a << std::endl;
	}

	void _onZoom(int a) {
		std::cout << this << " onZoom: " << a << std::endl;
	}

public:

	/// bind with lambda...
	slot<int> onBar { [this](int a) { std::cout << this << " lambda: " << a << std::endl; } };

	/// or with bind...
	slot<int> onFoo { std::bind(&B::_onFoo, this, std::placeholders::_1) };

	/// or with member function matching the signal signatures.
	slot<int> onZoom { USIG_MBIND(&B::_onZoom, this) }; // { mbind<&B::_onZoom>(this) }; //

	B() {}

	/// you can clone the connections on copy
	B(B const &o) {
		onFoo.clone_connections(o.onFoo);
		onBar.clone_connections(o.onBar);
		onZoom.clone_connections(o.onZoom);
	}
};

}


void basicTest() {
	std::cout << ".." << std::endl;

	signal<int> s_Foo;

	{
		slot<int> freeSlot([](int a) { std::cout << "free slot: " << a << std::endl; });
		s_Foo.connect(freeSlot);
		s_Foo(23);
		std::cout << ".." << std::endl;
	}

	enc_ns::B *b1 = new enc_ns::B;


	s_Foo.connect(b1->onFoo);
	s_Foo.connect(b1->onBar);
	s_Foo.connect(b1->onZoom);

	enc_ns::B b2(*b1);

	s_Foo(1);
	std::cout << ".." << std::endl;

	s_Foo.disconnect(b2.onBar);

	delete b1;

	s_Foo(2);
	std::cout << ".." << std::endl;

	{
		signal<int> s_Bar;
		s_Bar.connect(b2.onZoom);
		s_Bar(48);
		std::cout << ".." << std::endl;
	}

	s_Foo(29);
	std::cout << ".." << std::endl;
}

void threadTest() {
	slot<int> freeSlot([](int a) { std::cout << a << std::endl; } );

	std::vector<std::thread> ts;

	for (int tid = 0; tid < 25; tid++) {
		auto tfunc = [&freeSlot, tid] {
			{
				signal<int> s_Foo;
				s_Foo.connect(freeSlot);

				std::this_thread::sleep_for(std::chrono::milliseconds(2));

				for (int i = 0; i < 400; i++) {
					s_Foo(tid);
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
			}
		};

		ts.emplace_back(tfunc);
	}

	for (std::thread &t : ts) {
		t.join();
	}

}

int main() {

	basicTest();

	//threadTest();


	return 0;
}