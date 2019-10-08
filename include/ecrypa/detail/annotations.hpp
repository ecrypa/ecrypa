#pragma once

#include <type_traits>

#include <ecrypa/detail/panics.hpp>

namespace ecrypa::detail {

////////////////////////////////////////////////////////////////////////////////

// Annotation of nonstatic nonreference data members: `a(member_ptr, "name")`
// ```
// class FooBarBaz {
//   int foo_ = 4;
//   int bar_ = 42;
//   double baz_ = 7.4;
//
//   template<class A> friend constexpr auto annotate(A a, FooBarBaz* /*adl*/) {
//     return a(                      // <- see `annotator<Outer>::operator()`
//       a(&FooBarBaz::foo_, "foo_"), // <- 1st member object pointer
//       a(&FooBarBaz::bar_, "bar_"), // <- 2nd
//       a(&FooBarBaz::baz_, "baz_")  // <- 3rd
//     );
//   }
// };
// ```

template<class Outer, class Inner>
struct nonref_member_annotation {
  using inner_type = Inner;

  Inner Outer::* member_obj_ptr;
  const char* name;
};

template<class Outer, class Inner>
constexpr auto make_annotation(nonref_member_annotation<Outer, Inner> nrma) {
  return nrma;
}

////////////////////////////////////////////////////////////////////////////////

// Annotation of base classes: pass value-initialized pointer to base class
// ```
// struct PublicBase {};
// struct ProtectedBase {};
// struct PrivateBase {};
//
// class Derived
//   : public PublicBase
//   , protected ProtectedBase
//   , private PrivateBase
// {
//   FooBarBaz fbb_{};
//
//   template<class A> friend constexpr auto annotate(A a, Derived* /*adl*/) {
//     return a(
//       (PublicBase*){},                   // <- value-initialized pointer
//       (ProtectedBase*){},                // <- dito
//       static_cast<PrivateBase*>(nullptr),// <- alternate verbose syntax
//       a(&Derived::fbb_, "fbb_")          // <- combine with member annotation
//     );
//   }
// };
// ```

template<class Derived, class Base>
struct base_annotation {
  using inner_type = Base;
};

template<class Derived, class Base>
constexpr auto make_annotation(Base*)
  -> std::enable_if_t<
    std::is_base_of_v<Base, Derived>
    && std::is_same_v<std::decay_t<Base>, Base>
    && !std::is_same_v<Base, Derived>
    , base_annotation<Derived, Base>
  >
{ return {}; }

////////////////////////////////////////////////////////////////////////////////

// Annotation of nonstatic reference data member: `a(accessor_lambda, "name")`
// ```
// class RefDemo {
//   int i_ = 4;
//
//   int& lvalue_ = i_;
//   const int& clvalue_ = i_;
//   int&& rvalue_ = std::move(i_);
//   const int&& crvalue_ = std::move(i_);
//
//   template<class A> friend constexpr auto annotate(A a, RefDemo* /*adl*/) {
//     return a(
//       a(&RefDemo::i_, "i_"),      // <- combine with nonref member annotation
//       a([] (auto self) { return A::lref(self->lvalue_); },  "lvalue_"),
//       a([] (auto self) { return A::lref(self->clvalue_); }, "clvalue_"),
//       a([] (auto self) { return A::rref(self->rvalue_); },  "rvalue_"),
//       a([] (auto self) { return A::rref(self->crvalue_); }, "crvalue_")
//     );
//   }
// };
// ```

template<class T> struct rref_wrapper { using inner_type = T&&; T&& ref; };
template<class T> struct lref_wrapper { using inner_type = T&; T& ref; };

template<class Wrapper> struct is_ref_wrapper : std::false_type {};
template<class T> struct is_ref_wrapper<rref_wrapper<T>> : std::true_type {};
template<class T> struct is_ref_wrapper<lref_wrapper<T>> : std::true_type {};
template<class W> constexpr bool is_ref_wrapper_v = is_ref_wrapper<W>::value;

template<
  class Outer,
  class Accessor,
  class = std::enable_if_t<std::is_empty_v<Accessor>>,
  class Wrapper = std::invoke_result_t<Accessor, const Outer*>,
  class = std::enable_if_t<is_ref_wrapper_v<Wrapper>>>
struct ref_member_annotation : Accessor {// empty-base optimization
  using inner_type = typename Wrapper::inner_type;
  const char* name;
};

template<class Outer, class Accessor>
constexpr auto make_annotation(ref_member_annotation<Outer, Accessor> rma) {
  return rma;
}

////////////////////////////////////////////////////////////////////////////////

}// ecrypa::detail
