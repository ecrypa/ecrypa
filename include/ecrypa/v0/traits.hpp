#pragma once

#include <ecrypa/detail/traits.hpp>

// TODO: Is inheritance better than aliasing?

namespace ecrypa {
inline namespace v0 {

////////////////////////////////////////////////////////////////////////////////

template<class Outer>
using is_annotated =
  detail::is_annotated<Outer>;

template<class Outer>
using is_annotated_t = typename is_annotated<Outer>::type;

////////////////////////////////////////////////////////////////////////////////

template<class From, class To>
using is_c_style_cast_convertible =
  detail::is_c_style_cast_convertible<From, To>;

////////////////////////////////////////////////////////////////////////////////

template<class Base, class Derived>
using is_unambiguous_base_of =
  detail::is_unambiguous_base_of<Base, Derived>;

template<class Base, class Derived>
using is_ambiguous_base_of =
  detail::is_ambiguous_base_of<Base, Derived>;

////////////////////////////////////////////////////////////////////////////////

template<class Base, class Derived>
using is_unambiguous_public_base_of =
  detail::is_unambiguous_public_base_of<Base, Derived>;

template<class Base, class Derived> using
is_unambiguous_protected_base_of_nonfinal =
  detail::is_unambiguous_protected_base_of_nonfinal<Base, Derived>;

template<class Base, class Derived>
using is_unambiguous_effectively_private_base_of =
  detail::is_unambiguous_effectively_private_base_of<Base, Derived>;

////////////////////////////////////////////////////////////////////////////////

// TODO: categorize into detail vs non-detail
constexpr auto use_ecrypa_serialization(...) -> std::false_type;

template<class Outer>
constexpr auto has_ecrypa_serialization_(Outer* outer)
  -> std::conjunction<
    is_annotated<Outer>,
    std::is_same<decltype(use_ecrypa_serialization(outer)), std::true_type>
  >;

template<class Outer_, class Outer = std::decay_t<Outer_>>
struct has_ecrypa_serialization
  : decltype( has_ecrypa_serialization_(std::declval<Outer*>()) )
{};

////////////////////////////////////////////////////////////////////////////////

}// inline v0
}// ecrypa
