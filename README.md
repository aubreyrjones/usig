usig
====

usig is a tiny, header-only C++11 signals/slots implementation. It depends only on the C++ standard library.

usig is licensed under the MIT License, making it compatible for inclusion with both closed- and open-source work. You
are encouraged to give back what you can.

Note: in the long tradition of using 'u' to represent 'µ', the correct pronunciation of the project's name is "mew sig". 
Still, don't be a dick just 'cause somebody says "yoo sig".


Building usig
-------------

There's nothing to build, just copy `usig.h` into your project and include it.


Basics
------

usig defines two concepts: signals, and slots.

Signals are function-like objects that broadcast to all subscribed slots when called.

Slots are function-like objects that forward to some action function when called. They serve as an intermediate between
signals and actions so that connections can be tracked and automatically removed when slot- and signal-owning objects
are destroyed.

Both slots and signals are threadsafe. Furthermore, each slot and each signal acts as a monitor, permitting only a
single invocation of itself at a time. This means that, for instance, two different threads attempting to call the same
signal simultaneously will be serialized. Additionally, it means that no two signals (or invocations of the same signal)
can call the same slot simultaneously. While this reduces parallelism, it ensures that signal graph updates are atomic.

Note: a future version of usig might offer configurable threading models. As it stands, I don't need it yet, so I
haven't built it.

Usage
-----

    struct SignalOwningStruct {
        usig::signal<int> s_SomeSignal;
    }
    
    struct SlotOwningStruct {
        usig::slot<int> onSomeSignal { [](int x) -> void { cout << x << endl; } };
        
        SlotOwningStruct(SlotOwningStruct const& o) {
            // slots are not copy-constructable, since you need to figure out what to do with connected signals
            this->onSomeSignal.clone_connections(o.onSomeSignal);
        }
    }
    
    int main() {
        SignalOwningStruct sig;
        SlotOwningStruct slt;
        
        usig::connect(slt.onSomeSignal, sig.s_SomeSignal);
        
        sig.s_SomeSignal(23);
    }

For a full set of tests and examples, check out `main.cpp` in this repository.


ufrp
----

There are a series of `ufrpN.h` and example files in the repository. These are a series of (attempted) implementations
of functional-reactive programming using usig. I wouldn't count on these yet.