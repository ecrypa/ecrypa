#pragma once

#include <string>
#include <utility>

#include <ecrypa/v0/items.hpp>

namespace ecrypa {
inline namespace v0 {

////////////////////////////////////////////////////////////////////////////////

template<class T, class... Tags>
struct adl_tagged {
  T name_;
  /*implicit*/ constexpr operator T() const { return static_cast<T>(name_); }
};

////////////////////////////////////////////////////////////////////////////////

template<class Archive, class T>
constexpr auto make_nvp(Archive&, const char* name, T&& value) {
  return make_nvp(
    adl_tagged<const char*, Archive>{name},
    std::forward<T>(value)
  );
}

template<class Archive, class T>
constexpr auto make_nvp(Archive& archive, const std::string& name, T&& value) {
  return make_nvp(archive, name.c_str(), std::forward<T>(value));
}

////////////////////////////////////////////////////////////////////////////////

}// inline v0
}// ecrypa
