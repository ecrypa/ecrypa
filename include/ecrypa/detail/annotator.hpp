#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

#include <ecrypa/detail/annotations.hpp>

namespace ecrypa::detail {

////////////////////////////////////////////////////////////////////////////////

template<class Outer>
struct annotator {
  static_assert(std::is_same<std::decay_t<Outer>, Outer>{});

// collect annotations in tuple
  template<class... Annotations>
  constexpr auto operator()(Annotations... annotations) const
    -> decltype( std::make_tuple(make_annotation<Outer>(annotations)...) )
  { return       std::make_tuple(make_annotation<Outer>(annotations)...); }

// pair reference-member name with its accessor
  template<class Accessor>
  constexpr auto operator()(Accessor accessor, const char* name) const
    ->     ref_member_annotation<Outer, Accessor>
  { return ref_member_annotation<Outer, Accessor>{accessor, name}; }

  template<class Accessor>
  constexpr auto operator()(const char* name, Accessor accessor) const
    ->     ref_member_annotation<Outer, Accessor>
  { return ref_member_annotation<Outer, Accessor>{accessor, name}; }

  template<class Dependent, class Result>
  struct hack_ {
    using type = Result;
  };

  template<class... Ts>
  using hack = typename hack_<Ts...>::type;

  template<class Inner>
  constexpr auto operator()(Inner hack<Inner, Outer>::* member_ptr, const char* name) const
    ->     nonref_member_annotation<Outer, Inner>
  { return nonref_member_annotation<Outer, Inner>{member_ptr, name}; }

// freeze the value category of reference members
  template<class Ref> static constexpr auto rref(Ref& ref)
  { return rref_wrapper<std::remove_reference_t<Ref>>{std::move(ref)}; }

  template<class Ref> static constexpr auto lref(Ref& ref)
  { return lref_wrapper<std::remove_reference_t<Ref>>{ref}; }
};

////////////////////////////////////////////////////////////////////////////////

template<class Outer>
constexpr auto feed_annotator_into()
  -> decltype( annotate(annotator<Outer>{}, static_cast<Outer*>(nullptr)) )
{ return       annotate(annotator<Outer>{}, static_cast<Outer*>(nullptr)); }

////////////////////////////////////////////////////////////////////////////////

}// ecrypa::detail
