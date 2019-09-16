#pragma once

#include <string_view>

#include <ecrypa/detail/names.hpp>

namespace ecrypa {
inline namespace v0 {

////////////////////////////////////////////////////////////////////////////////

template<class T>
constexpr std::string_view type_name() {
  return detail::type_name<T>;
}

////////////////////////////////////////////////////////////////////////////////

template<class Outer, class Inner, Inner Outer::* member_ptr>
constexpr std::string_view member_name() {
  return detail::member_name<Outer, Inner, member_ptr>;
}

template<class MemberPtr, MemberPtr member_ptr>
constexpr std::string_view member_name() {
  return detail::get_member_name<MemberPtr, member_ptr>(member_ptr);
}

template<auto member_ptr>
constexpr std::string_view member_name() {
  return detail::get_member_name<decltype(member_ptr), member_ptr>(member_ptr);
}

////////////////////////////////////////////////////////////////////////////////

}// inline v0
}// ecrypa
