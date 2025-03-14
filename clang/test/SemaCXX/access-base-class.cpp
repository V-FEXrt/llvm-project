// RUN: %clang_cc1 -fsyntax-only -verify %s
namespace T1 {

class A { };
class B : private A { }; // expected-note {{declared private here}}

void f(B* b) {
  A *a = b; // expected-error{{cannot cast 'B' to its private base class 'A'}}
}

}

namespace T2 {

class A { };
class B : A { }; // expected-note {{implicitly declared private here}}

void f(B* b) {
  A *a = b; // expected-error {{cannot cast 'B' to its private base class 'A'}}
}

}

namespace T3 {

class A { };
class B : public A { };

void f(B* b) {
  A *a = b;
}

}

namespace T4 {

class A {};

class B : private virtual A {};
class C : public virtual A {};

class D : public B, public C {};

void f(D *d) {
  // This takes the D->C->B->A path.
  A *a = d;
}

}

namespace T5 {
  class A {};

  class B : private A {
    void f(B *b) {
      A *a = b;
    }
  };
}

namespace T6 {
  class C;

  class A {}; // expected-note{{member is declared here}}

  class B : private A { // expected-note {{declared private here}} expected-note {{constrained by private inheritance here}}
    void f(C* c);
  };

  class C : public B {
    void f(C *c) {
      A* a = c; // expected-error {{cannot cast 'C' to its private base class 'A'}} \
                // expected-error {{'A' is a private member of 'T6::A'}}
    }
  };

  void B::f(C *c) {
    A *a = c;
  }
}

namespace T7 {
  class A {};
  class B : public A {};
  class C : private B {
    void f(C *c) {
      A* a = c; // okay
    }
  };
}

namespace T8 {
template <int>
struct flag {
  static constexpr bool value = true;
};

template <class T>
struct trait : flag<sizeof(T)> {}; // expected-note 2{{here}}

template <class T, bool Inferred = trait<T>::value> // expected-note 2{{here}}
struct a {};

template <class T>
class b {
  a<T> x; // expected-note {{here}}
  using U = a<T>;
};

template <int>
struct Impossible {
  static_assert(false, ""); // expected-error {{static assertion failed}}
};

// verify "no member named 'value'" bogus diagnostic is not emitted.
trait<b<Impossible<0>>>::value; // expected-note {{here}}
} // namespace T8
