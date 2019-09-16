#pragma once

#include <type_traits>
#include <utility>

#include <ecrypa/detail/annotator.hpp>

namespace ecrypa::detail {

////////////////////////////////////////////////////////////////////////////////

template<class Outer, class ExpreesionSfinae = void>
struct is_annotated : std::false_type {
  static_assert(std::is_same<std::decay_t<Outer>, Outer>{});
};

template<class Outer>
struct is_annotated<
  Outer,
  std::void_t<decltype(   ::ecrypa::detail::feed_annotator_into<Outer>()   )>
> : std::true_type {
  static_assert(std::is_same<std::decay_t<Outer>, Outer>{});
};

////////////////////////////////////////////////////////////////////////////////

template<class Src, class Dst, class ExpressionSfinae = void>
struct is_c_style_cast_convertible : std::false_type {};

template<class Src, class Dst>
struct is_c_style_cast_convertible<
  Src, Dst,
  std::void_t<decltype(   (Dst)std::declval<Src>()   )>
> : std::true_type {};

////////////////////////////////////////////////////////////////////////////////

template<class Base, class Derived>
struct is_unambiguous_base_of : std::bool_constant<
  std::is_base_of<Base, Derived>{}
  && !std::is_same<Base, Derived>{}
  && is_c_style_cast_convertible<Derived*, Base*>{}
> {
  static_assert(std::is_same<std::decay_t<Base>, Base>{});
  static_assert(std::is_same<std::decay_t<Derived>, Derived>{});
};

template<class Base, class Derived>
struct is_ambiguous_base_of : std::bool_constant<
  std::is_base_of<Base, Derived>{}
  && !std::is_same<Base, Derived>{}
  && !is_c_style_cast_convertible<Derived*, Base*>{}
> {
  static_assert(std::is_same<std::decay_t<Base>, Base>{});
  static_assert(std::is_same<std::decay_t<Derived>, Derived>{});
};

////////////////////////////////////////////////////////////////////////////////

template<
  class Base,
  class Derived,
  bool can_derive = std::is_class_v<Derived> && !std::is_final_v<Derived>
> struct protected_base_helper {
  static_assert(can_derive == false, "see specialization");
  static constexpr bool is_convertible = false;
};

template<class Base, class Derived>
struct protected_base_helper<Base, Derived, true>
  : Derived// <- !!!
{
  template<class Src, class Dst, class ExpressionSfinae = void>
  struct is_convertible_ : std::false_type {};

  template<class Src, class Dst>
  struct is_convertible_<
    Src, Dst,
    std::void_t<decltype(   static_cast<Dst>(std::declval<Src>())   )>
  > : std::true_type {};

  static constexpr auto is_convertible = is_convertible_<Derived*, Base*>{};
};

template<class Base, class Derived>
struct is_unambiguous_public_base_of : std::bool_constant<
  is_unambiguous_base_of<Base, Derived>{}
  && std::is_convertible<Derived*, Base*>{}
> {};

template<class Base, class Derived>
struct is_unambiguous_protected_base_of_nonfinal : std::bool_constant<
  is_unambiguous_base_of<Base, Derived>{}
  && !std::is_convertible<Derived*, Base*>{}
  && protected_base_helper<Base, Derived>::is_convertible
> {};

template<class Base, class Derived>
struct is_unambiguous_effectively_private_base_of : std::bool_constant<
  is_unambiguous_base_of<Base, Derived>{}
  && !std::is_convertible<Derived*, Base*>{}
  && !protected_base_helper<Base, Derived>::is_convertible
> {};

////////////////////////////////////////////////////////////////////////////////

}// ecrypa::detail
