#pragma once

#include <variant>

#include <ecrypa/v0/items.hpp>
#include <ecrypa/v0/make_nvp.hpp>
#include <ecrypa/v0/traits.hpp>

namespace ecrypa {
inline namespace v0 {

////////////////////////////////////////////////////////////////////////////////

template<
  class Archive,
  class Outer,
  class = std::enable_if_t<has_ecrypa_serialization<Outer>{}>
> auto serialize(Archive& archive, Outer& outer) {
  apply_items<Outer>([&] (auto... bms) {
    archive(
      ecrypa::make_nvp(archive, bms.inner_name(), bms(outer))...
    );
  });
}

template<class Outer, std::size_t... is>
constexpr auto get_accessor(std::index_sequence<is...>, std::string_view name) {
  static_assert(is_annotated<Outer>{});
  using Ret = std::variant<std::monostate, item<is, Outer>...>;
  Ret ret{};
  each_member<Outer>([&] (auto member_accessor) {
    constexpr const char* inner_name = member_accessor.inner_name();
    if(std::string_view{inner_name} == name) {
      ret = member_accessor;
    }
  });
  return ret;
}

template<class Outer>
constexpr auto get_accessor(std::string_view name) {
  using D = std::decay_t<Outer>;
  static_assert(is_annotated<D>{});
  return get_accessor<D>(members<D>::ind_seq, name);
}

template<
  class Outer,
  class = std::enable_if_t<is_annotated<Outer>{}>
>
constexpr bool operator==(
  const Outer& lhs,
  const Outer& rhs
) {
  return apply_items<Outer>([&] (auto... bms) {
    return (... && (bms(lhs) == bms(rhs)));
  });
}

////////////////////////////////////////////////////////////////////////////////

}// inline v0
}// ecrypa
