#pragma once

#include <cstddef>

#include <tuple>
#include <type_traits>

#include <ecrypa/detail/annotator.hpp>
#include <ecrypa/detail/panics.hpp>
#include <ecrypa/detail/traits.hpp>

namespace ecrypa::detail {

////////////////////////////////////////////////////////////////////////////////

template<class Outer>
struct annotation_tuple {
  static_assert(std::is_same<std::decay_t<Outer>, Outer>{});

  static_assert(
    panic_annotation_not_found<Outer>
    ::unless(is_annotated<Outer>{})
  );

  static constexpr auto value = ::ecrypa::detail::feed_annotator_into<Outer>();
  using type = std::decay_t<decltype(value)>;
  static constexpr auto size = std::tuple_size<type>{};
};

////////////////////////////////////////////////////////////////////////////////

template<
  std::size_t idx,
  class Outer,
  class Tuple = annotation_tuple<Outer>
>
struct annotation_tuple_element {
  static_assert(
    panic_idx_out_of_range<idx, Outer, Tuple::size>::template
    unless<(idx < Tuple::size)>()
  );

  using type = std::tuple_element_t<idx, typename Tuple::type>;
  static constexpr type get() { return std::get<idx>(Tuple::value); }
};

template<std::size_t idx, class Outer>
using annotation_tuple_element_t =
  typename annotation_tuple_element<idx, Outer>::type;

////////////////////////////////////////////////////////////////////////////////

}// ecrypa::detail
