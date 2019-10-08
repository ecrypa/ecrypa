#pragma once

#include <cassert>
#include <cstddef>

#include <array>
#include <string_view>
#include <utility>

#include <ecrypa/detail/traits.hpp>

namespace ecrypa::detail {

////////////////////////////////////////////////////////////////////////////////

template<std::size_t size_>
class cx_string {
 private:
  std::string_view view_;
  const char storage_[size_ + 1];

  template<std::size_t... is>
  constexpr cx_string(std::string_view sv, std::index_sequence<is...>)
    : view_{sv}
    , storage_{view_[is]..., '\0'}
  { assert(sv == view_); }

 public:
  constexpr cx_string(std::string_view sv)
    : cx_string(sv, std::make_index_sequence<size_>{})
  { assert(sv.size() == size_); }

  constexpr cx_string(const char(&char_arr)[size_+1])
    : cx_string(std::string_view{char_arr})
  { assert(char_arr[size_] == '\0'); }

  static constexpr std::size_t size() { return size_; }

  using ConstCharArray = const char[size_ + 1];
  /*implicit*/ constexpr operator ConstCharArray&() const { return storage_; }
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
constexpr auto view_type_name() {
  return remove_prefixes_and_suffixes(
#if defined(__clang__)
    {"auto ecrypa::detail::view_type_name() [T = "},
    __PRETTY_FUNCTION__,
    {"]"}
#elif defined(__GNUC__)
    {"constexpr auto ecrypa::detail::view_type_name() [with T = "},
    __PRETTY_FUNCTION__,
    {"]"}
#else
#error UNSUPPORTED COMPILER
    {""},
    "",
    {""}
#endif
  );
}

template<
  class T,
  std::size_t size = view_type_name<T>().size(),
  class Is = std::make_index_sequence<size>>
class type_name_;

template<class T, std::size_t size_, std::size_t... is>
class type_name_<T, size_, std::index_sequence<is...>> {
 private:
  static_assert(
    ((std::index_sequence<is...>{} = std::make_index_sequence<size_>{}), 1), "!"
  );

// allows the return type `arr<const char, size_ + 1>&` (note the reference)
  template<class C, std::size_t N> using arr = C[N];

// static data member `view_` mitigates compiler warning `-Wstringop-overflow`
  static constexpr std::string_view view_ = view_type_name<T>();
  static constexpr arr<const char, size_ + 1> storage_{view_[is]..., '\0'};

 public:
  static constexpr std::size_t size() { return size_; }
  static constexpr std::string_view view() { return {storage_, size_}; }
  static constexpr arr<const char, size_ + 1>& c_arr() { return storage_; }
  static constexpr const char* c_str() { return storage_; }

  /*implicit*/ constexpr operator arr<const char, size_ + 1>&() const {
    return storage_;
  }
};

template<class T> constexpr type_name_<T> type_name{};

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
constexpr const char* get_member_name(Inner Outer::* /*deduction_only*/) {
  static_assert(std::is_same<Inner Outer::*, MemberPtr>{});
  auto& storage = member_name<Outer, Inner, member_obj_ptr>;
  const char* ret = storage;
//  static_assert(ret[storage.size()] == '\0');
  return member_name<Outer, Inner, member_obj_ptr>;
}

////////////////////////////////////////////////////////////////////////////////

template<class Base, class Derived>
constexpr const char* make_base_name_for_derived() {
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
constexpr const char* base_name_for_derived =
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
