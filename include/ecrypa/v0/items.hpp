#pragma once

#include <cstddef>

#include <string_view>
#include <type_traits>
#include <utility>

#include <ecrypa/v0/names.hpp>
#include <ecrypa/detail/items.hpp>
#include <ecrypa/detail/utils.hpp>

// TODO: Design a good interface.
//
// (For now, just expose the item handles (bases and members) together with
// `each` and `apply` helpers.)

namespace ecrypa {
inline namespace v0 {

////////////////////////////////////////////////////////////////////////////////

template<std::size_t idx_, class Outer>
class item : private detail::item_impl_t<idx_, Outer> {
 private:
  using Impl = detail::item_impl_t<idx_, Outer>;
  using O = Outer;
  using I = typename Impl::inner_type;

  static_assert(detail::assert_item_validity<Outer>());

 public:
  static constexpr std::size_t idx = idx_;
  using inner_type = I;
  using outer_type = O;

  static constexpr std::string_view inner_name() { return Impl::inner_name(); }
  static constexpr std::string_view inner_type_name() { return type_name<I>(); }
  static constexpr std::string_view outer_type_name() { return type_name<O>(); }

  using Impl::operator();

  static constexpr auto is_base = Impl::is_base;
  static constexpr auto is_member = Impl::is_member;
};

////////////////////////////////////////////////////////////////////////////////

template<class Outer, class F, std::size_t... inds>
constexpr decltype(auto) apply(F&& f, std::index_sequence<inds...>) {
  return f(::ecrypa::item<inds, Outer>{}...);
}

template<class Outer, class F, std::size_t... inds>
constexpr void each(F&& f, std::index_sequence<inds...>) {
  (..., f(::ecrypa::item<inds, Outer>{}));
}

////////////////////////////////////////////////////////////////////////////////

template<class Outer>
struct items {
  static constexpr auto count = detail::annotation_tuple<Outer>::size;
  static constexpr auto ind_seq = std::make_index_sequence<count>{};

  template<class F>
  static constexpr decltype(auto) apply(F&& f) {
    return ::ecrypa::apply<Outer>(std::forward<F>(f), ind_seq);
  }
  template<class F>
  static constexpr void each(F&& f) {
    ::ecrypa::each<Outer>(std::forward<F>(f), ind_seq);
  }

  static_assert(
    apply([] (auto... bms) { return (... && (bms.is_base != bms.is_member)); }),
    "item: either a base or a member"
  );
};

////////////////////////////////////////////////////////////////////////////////

template<class Outer>
struct bases {
 private:
  static constexpr auto count_ = items<Outer>::apply(
    [] (auto... bms) { return (std::size_t(0) + ... + bms.is_base); });

 public:
  static constexpr auto count = std::integral_constant<std::size_t, count_>{};
  static constexpr auto ind_seq = std::make_index_sequence<count>{};

  template<class F>
  static constexpr decltype(auto) apply(F&& f) {
    return ::ecrypa::apply<Outer>(std::forward<F>(f), ind_seq);
  }
  template<class F>
  static constexpr void each(F&& f) {
    ::ecrypa::each<Outer>(std::forward<F>(f), ind_seq);
  }

  static_assert(apply([] (auto... bs) { return (... && bs.is_base); }));
};

////////////////////////////////////////////////////////////////////////////////

template<class Outer>
struct members {
 private:
  static constexpr auto count_ = items<Outer>::apply(
    [] (auto... bms) { return (std::size_t(0) + ... + bms.is_member); });

 public:
  static constexpr auto count = std::integral_constant<std::size_t, count_>{};
  static constexpr auto ind_seq = detail::incr_inds_by(
    std::make_index_sequence<count>{}, bases<Outer>::count
  );

  template<class F>
  static constexpr decltype(auto) apply(F&& f) {
    return ::ecrypa::apply<Outer>(std::forward<F>(f), ind_seq);
  }
  template<class F>
  static constexpr void each(F&& f) {
    ::ecrypa::each<Outer>(std::forward<F>(f), ind_seq);
  }

  static_assert(apply([] (auto... ms) { return (... && ms.is_member); }));
};

////////////////////////////////////////////////////////////////////////////////

template<class Outer, class F> constexpr decltype(auto) apply_items(F&& f)
{ return items<Outer>::apply(std::forward<F>(f)); }
template<class Outer, class F> constexpr decltype(auto) apply_bases(F&& f)
{ return bases<Outer>::apply(std::forward<F>(f)); }
template<class Outer, class F> constexpr decltype(auto) apply_members(F&& f)
{ return members<Outer>::apply(std::forward<F>(f)); }

template<class Outer, class F> constexpr void each_item(F&& f)
{ items<Outer>::each(std::forward<F>(f)); }
template<class Outer, class F> constexpr void each_base(F&& f)
{ bases<Outer>::each(std::forward<F>(f)); }
template<class Outer, class F> constexpr void each_member(F&& f)
{ members<Outer>::each(std::forward<F>(f)); }

////////////////////////////////////////////////////////////////////////////////

/*

template<std::size_t idx, class Outer>
constexpr auto get_item_handle() {
  static_assert(idx < items<Outer>::count);
  return item<idx, Outer>{};
}
template<std::size_t idx, class Outer>
constexpr auto get_base_handle() {
  static_assert(idx < bases<Outer>::count);
  return item<idx, Outer>{};
}
template<std::size_t idx, class Outer>
constexpr auto get_member_handle() {
  static_assert(idx < members<Outer>::count);
  return item<bases<Outer>::count + idx, Outer>{};
}

template<std::size_t idx, class Outer>
constexpr decltype(auto) get_item(Outer&& outer) {
  constexpr auto bm = get_item_handle<idx, std::decay_t<Outer>>();
  using Ret =    decltype( bm(std::forward<Outer>(outer)) );
  return static_cast<Ret>( bm(std::forward<Outer>(outer)) );
}
template<std::size_t idx, class Outer>
constexpr decltype(auto) get_base(Outer&& outer) {
  constexpr auto b = get_base_handle<idx, std::decay_t<Outer>>();
  using Ret =    decltype( b(std::forward<Outer>(outer)) );
  return static_cast<Ret>( b(std::forward<Outer>(outer)) );
}
template<std::size_t idx, class Outer>
constexpr decltype(auto) get_member(Outer&& outer) {
  constexpr auto m = get_member_handle<idx, std::decay_t<Outer>>();
  using Ret =    decltype( m(std::forward<Outer>(outer)) );
  return static_cast<Ret>( m(std::forward<Outer>(outer)) );
}

template<std::size_t idx, class Outer>
constexpr std::string_view get_item_type_name() {
  return get_item_handle<idx, Outer>().inner_type_name();
}
template<std::size_t idx, class Outer>
constexpr std::string_view get_base_type_name() {
  return get_base_handle<idx, Outer>().inner_type_name();
}
template<std::size_t idx, class Outer>
constexpr std::string_view get_member_type_name() {
  return get_member_handle<idx, Outer>().inner_type_name();
}

template<std::size_t idx, class Outer>
constexpr std::string_view get_member_name() {
  return get_member_handle<idx, Outer>().inner_name();
}

template<std::size_t idx, class Outer> using item_type_t =
  typename decltype(get_item_handle<idx, Outer>())::inner_type;
template<std::size_t idx, class Outer> using base_type_t =
  typename decltype(get_base_handle<idx, Outer>())::inner_type;
template<std::size_t idx, class Outer> using member_type_t =
  typename decltype(get_member_handle<idx, Outer>())::inner_type;

*/

////////////////////////////////////////////////////////////////////////////////

}// inline v0
}// ecrypa
