#pragma once

#include <ecrypa/detail/traits.hpp>

// TODO: Is inheritance better than aliasing?

namespace ecrypa {
inline namespace v0 {

////////////////////////////////////////////////////////////////////////////////

template<class Outer>
using is_annotated =
  detail::is_annotated<Outer>;

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

}// inline v0
}// ecrypa
