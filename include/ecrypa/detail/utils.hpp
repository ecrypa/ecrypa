#pragma once

#include <cstddef>

#include <type_traits>
#include <utility>

namespace ecrypa::detail {

template<std::size_t offset, std::size_t... inds>
constexpr auto incr_inds_by(
  std::index_sequence<inds...>,
  std::integral_constant<std::size_t, offset> = {}
) {
  return std::index_sequence<(inds + offset)...>{};
}

}// ecrypa::detail
