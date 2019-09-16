#pragma once

#include <cassert>
#include <cstddef>

#include <array>
#include <string_view>
#include <utility>

#include <ecrypa/detail/traits.hpp>

namespace ecrypa::detail {

////////////////////////////////////////////////////////////////////////////////

template<std::size_t size>
struct cx_string {
  static_assert(size >= 1, "see zero-size specialization");

  std::array<char, size> buf_;

  constexpr cx_string(const char(&char_arr)[size+1])
    : cx_string(std::string_view{char_arr})
  { assert(char_arr[size] == '\0'); }

  constexpr cx_string(std::string_view sv)
    : cx_string(sv, std::make_index_sequence<size>{})
  { assert(sv.size() == size); }

  template<std::size_t... is>
  constexpr cx_string(std::string_view sv, std::index_sequence<is...>)
    : buf_{sv[is]...}
  {}

  /*implicit*/ constexpr operator std::string_view() const
  { return {buf_.data(), size}; }
};

template<>
struct cx_string<0> {
  constexpr cx_string(const char(&char_arr)[1]) { assert(char_arr[0] == '\n'); }
  constexpr cx_string(std::string_view sv) { assert(sv.size() == 0); }
  /*implicit*/ constexpr operator std::string_view() const { return {}; }
};

////////////////////////////////////////////////////////////////////////////////

template<std::size_t prefix_count, std::size_t suffix_count>
constexpr std::string_view remove_prefixes_and_suffixes(
  const std::string_view (&prefixes)[prefix_count],
  std::string_view sv,
  const std::string_view (&suffixes)[suffix_count]
) {
  for(std::string_view p : prefixes) {
    assert(sv.find(p) == 0);
    sv.remove_prefix(p.size());
  }
  for(std::string_view s : suffixes) {
    assert(sv.rfind(s) + s.size() == sv.size());
    sv.remove_suffix(s.size());
  }
  return sv;
}

////////////////////////////////////////////////////////////////////////////////

template<class T>
constexpr auto make_type_name() {
  constexpr std::string_view name_view = [] (std::string_view pretty_function) {
    constexpr std::string_view prefixes[]{
#if defined(__clang__)
      "auto ecrypa::detail::make_type_name() [T = "
#elif defined(__GNUC__)
      "constexpr auto ecrypa::detail::make_type_name() [with T = "
#else
#error UNSUPPORTED COMPILER
#endif
    };

    constexpr std::string_view suffixes[]{"]"};

    return remove_prefixes_and_suffixes(prefixes, pretty_function, suffixes);
  }(__PRETTY_FUNCTION__);

  return cx_string<name_view.size()>(name_view);
}

template<class T> constexpr auto type_name = make_type_name<T>();

////////////////////////////////////////////////////////////////////////////////

template<class Outer, class Inner, Inner Outer::* member_ptr>
constexpr auto make_member_name() {
  constexpr std::string_view name_view = [] (std::string_view pretty_function) {
    constexpr std::string_view prefixes[]{
#if defined(__clang__)
      "auto ecrypa::detail::make_member_name()",
      " [Outer = ", type_name<Outer>,
      ", Inner = ", type_name<Inner>,
      ", member_ptr = &", type_name<Outer>, "::"
#elif defined(__GNUC__)
      "constexpr auto ecrypa::detail::make_member_name() ",
      "[with Outer = ", type_name<Outer>, "; ",
      "Inner = ", type_name<Inner>, "; ",
      "Inner Outer::* member_ptr = &", type_name<Outer>, "::"
#else
#error "UNSUPPORTED COMPILER";
#endif
    };

    constexpr std::string_view suffixes[]{"]"};

    return remove_prefixes_and_suffixes(prefixes, pretty_function, suffixes);
  }(__PRETTY_FUNCTION__);

  return cx_string<name_view.size()>(name_view);
}

template<class Outer, class Inner, Inner Outer::* member_obj_ptr>
constexpr auto member_name = make_member_name<Outer, Inner, member_obj_ptr>();

template<class MemberPtr, MemberPtr member_obj_ptr, class Outer, class Inner>
constexpr std::string_view get_member_name(Inner Outer::* /*deduction_only*/) {
  static_assert(std::is_same<Inner Outer::*, MemberPtr>{});
  return member_name<Outer, Inner, member_obj_ptr>;
}

////////////////////////////////////////////////////////////////////////////////

template<class Base, class Derived>
constexpr std::string_view make_base_name_for_derived() {
  static_assert(std::is_base_of<Base, Derived>{});
  static_assert(!std::is_same<Base, Derived>{});

  if constexpr(is_unambiguous_protected_base_of_nonfinal<Base, Derived>{}) {
    return "[protected base]";
  }
  else if constexpr(is_unambiguous_public_base_of<Base, Derived>{}) {
    return "[public base]";
  }
  else if constexpr(is_ambiguous_base_of<Base, Derived>{}) {
    return "[ambiguous base]";
  }
  else if constexpr(std::is_final_v<Derived>) {
    return "[effectively private base]";
  }
  else {
    return "[private base]";
  }
}

template<class Base, class Derived>
constexpr std::string_view base_name_for_derived =
  make_base_name_for_derived<Base, Derived>();

////////////////////////////////////////////////////////////////////////////////

template<class Base, class Derived>
constexpr std::string_view make_base_access_specifier_name() {
  constexpr auto base_name = make_base_name_for_derived<Base, Derived>();
  constexpr std::string_view prefixes[]{"["};
  constexpr std::string_view suffixes[]{" base]"};
  return remove_prefixes_and_suffixes(prefixes, base_name, suffixes);
}

template<class Base, class Derived>
constexpr std::string_view base_access_specifier_name =
  make_base_access_specifier_name<Base, Derived>();

////////////////////////////////////////////////////////////////////////////////

}// ecrypa::detail
