// Annotation of nonstatic nonreference data members: `a(member_ptr, "name")`
class FooBarBaz {
  int foo_ = 4;
  int bar_ = 42;
  double baz_ = 7.4;

  template<class A> friend constexpr auto annotate(A a, FooBarBaz* /*adl*/) {
    return a(
      a(&FooBarBaz::foo_, "foo_"),
      a(&FooBarBaz::bar_, "bar_"),
      a(&FooBarBaz::baz_, "baz_")
    );
  }
};

// Annotation of base classes: pass value-initialized pointer to base class
struct PublicBase {};
struct ProtectedBase {};
struct PrivateBase {};

class Derived
  : public PublicBase
  , protected ProtectedBase
  , private PrivateBase
{
  FooBarBaz fbb_{};

  template<class A> friend constexpr auto annotate(A a, Derived* /*adl*/) {
    return a(
      (PublicBase*){},                   // <- value-initialized pointer
      (ProtectedBase*){},                // <- dito
      static_cast<PrivateBase*>(nullptr),// <- alternate verbose syntax
      a(&Derived::fbb_, "fbb_")          // <- combine with member annotation
    );
  }
};

// Annotation of nonstatic reference data member: `a(accessor_lambda, "name")`
class RefDemo {
  int i_ = 4;

  int& lvalue_ = i_;
  const int& clvalue_ = i_;
  int&& rvalue_ = static_cast<int&&>(i_);
  const int&& crvalue_ = static_cast<int&&>(i_);

  template<class A> friend constexpr auto annotate(A a, RefDemo* /*adl*/) {
    return a(
      a(&RefDemo::i_, "i_"),         // <- combine with nonref member annotation
      a([] (auto self) { return A::lref(self->lvalue_); },  "lvalue_"),
      a([] (auto self) { return A::lref(self->clvalue_); }, "clvalue_"),
      a([] (auto self) { return A::rref(self->rvalue_); },  "rvalue_"),
      a([] (auto self) { return A::rref(self->crvalue_); }, "crvalue_")
    );
  }
};

#include <iostream>

#include <ecrypa/ecrypa.hpp>

int main() {
  std::cout << "=== members ===" << std::endl;
  FooBarBaz fbb{};
  ecrypa::v0::each_member<FooBarBaz>([&] (auto item) {
    std::cout << item.inner_type_name() << " "
	      << item.outer_type_name() << "::"
	      << item.inner_name() << " => "
	      << item(fbb)
	      << std::endl;
  });
  std::cout << std::endl;

  Derived d{};
  std::cout << "=== bases and members ===" << std::endl;
  ecrypa::v0::each_item<Derived>([&] (auto item) {
    std::cout << item.inner_type_name() << " "
	      << item.outer_type_name() << "::"
	      << item.inner_name()
	      << std::endl;
  });
  std::cout << std::endl;

  RefDemo rd{};
  std::cout << "=== reference members ===" << std::endl;
  ecrypa::v0::each_member<RefDemo>([&] (auto item) {
    std::cout << item.inner_type_name() << " "
	      << item.outer_type_name() << "::"
	      << item.inner_name() << " => "
	      << item(rd)
	      << std::endl;
  });
  std::cout << std::endl;

  std::cout << ecrypa::v0::type_name<int>() << std::endl;
}
