#pragma once

#include <string_view>

#include <ecrypa/detail/names.hpp>

namespace ecrypa {
inline namespace v0 {

////////////////////////////////////////////////////////////////////////////////

template<class T>
constexpr const char* type_name() {
//  constexpr std::size_t size = detail::type_name<T>.size();
  return detail::type_name<T>;
}

////////////////////////////////////////////////////////////////////////////////

template<class Outer, class Inner, Inner Outer::* member_ptr>
constexpr const char* member_name() {
  return detail::get_member_name<Inner Outer::*, member_ptr>(member_ptr);
}

template<class MemberPtr, MemberPtr member_ptr>
constexpr const char* member_name() {
  return detail::get_member_name<MemberPtr, member_ptr>(member_ptr);
}

template<auto member_ptr>
constexpr const char* member_name() {
  return detail::get_member_name<decltype(member_ptr), member_ptr>(member_ptr);
}

////////////////////////////////////////////////////////////////////////////////

}// inline v0
}// ecrypa
