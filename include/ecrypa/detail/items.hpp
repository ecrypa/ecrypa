#pragma once

#include <cstddef>

#include <array>
#include <memory>
#include <string_view>
#include <type_traits>
#include <utility>

#include <ecrypa/detail/annotations.hpp>
#include <ecrypa/detail/annotation_tuple.hpp>
#include <ecrypa/detail/names.hpp>
#include <ecrypa/detail/panics.hpp>
#include <ecrypa/detail/traits.hpp>

namespace ecrypa::detail {

////////////////////////////////////////////////////////////////////////////////

template<
  std::size_t idx,
  class Outer,
  class Annotation = annotation_tuple_element_t<idx, Outer>
>
struct item_impl {
  static_assert(
    panic_unknown_annotation<Outer, Annotation>
    ::because_item_impl_needs_specialization()
  );
};

template<std::size_t idx, class Outer>
using item_impl_t = typename item_impl<idx, Outer>::type;

////////////////////////////////////////////////////////////////////////////////

template<std::size_t idx, class Derived, class Base>
class base_impl {
 public:
  using inner_type = Base;

  static_assert(
    panic_annotation_of_ambiguous_base<Derived, Base>
    ::in_case(
      is_ambiguous_base_of<Base, Derived>{}
    )
  );

  static constexpr const char* inner_name() {
    return base_name_for_derived<Base, Derived>;
  }

  constexpr Base& operator()(Derived& derived) const {
    return (Base&)derived;// must use c-style cast (bypasses access specifier)
  }
  constexpr const Base& operator()(const Derived& derived) const {
    return (const Base&)derived;// dito c-style cast
  }
  constexpr Base&& operator()(Derived&& derived) const {
    return (Base&&)derived;// dito c-style cast
  }
  constexpr const Base&& operator()(const Derived&& derived) const {
    return (const Base&&)derived;// dito c-style cast
  }

  static constexpr auto is_base = std::true_type{};
  static constexpr auto is_member = std::false_type{};
};

template<std::size_t idx, class Derived, class Base>
struct item_impl<idx, Derived, base_annotation<Derived, Base>> {
  using type = base_impl<idx, Derived, Base>;
};

////////////////////////////////////////////////////////////////////////////////

template<std::size_t idx, class Outer, class Inner>
class nonref_member_impl {
 private:
  static constexpr auto member_obj_ptr() {
    return annotation_tuple_element<idx, Outer>::get().member_obj_ptr;
  }

 public:
  using inner_type = Inner;

  static constexpr const char* inner_name() {
    return annotation_tuple_element<idx, Outer>::get().name;
  }

  constexpr inner_type& operator()(Outer& s) const {
    return s.*member_obj_ptr();
  }
  constexpr const inner_type& operator()(const Outer& s) const {
    return s.*member_obj_ptr();
  }
  constexpr inner_type&& operator()(Outer&& s) const {
    return std::move(s).*member_obj_ptr();
  }
  constexpr const inner_type&& operator()(const Outer&& s) const {
    return std::move(s).*member_obj_ptr();
  }

  static constexpr auto is_base = std::false_type{};
  static constexpr auto is_member = std::true_type{};
};

template<std::size_t idx, class Outer, class Inner>
struct item_impl<idx, Outer, nonref_member_annotation<Outer, Inner>> {
  using type = nonref_member_impl<idx, Outer, Inner>;
};

////////////////////////////////////////////////////////////////////////////////

template<std::size_t idx, class Outer, class Accessor>
struct ref_member_impl {
 private:
  static constexpr auto ref_annotation =
    annotation_tuple_element<idx, Outer>::get();

 public:
  using inner_type = typename decltype(ref_annotation)::inner_type;

  static constexpr const char* inner_name() { return ref_annotation.name; }

  constexpr inner_type operator()(const Outer& self) const {
    return static_cast<inner_type>( ref_annotation(std::addressof(self)).ref );
  }

  static constexpr auto is_base = std::false_type{};
  static constexpr auto is_member = std::true_type{};
};

template<std::size_t idx, class Outer, class Accessor>
struct item_impl<idx, Outer, ref_member_annotation<Outer, Accessor>> {
  using type = ref_member_impl<idx, Outer, Accessor>;
};

////////////////////////////////////////////////////////////////////////////////

template<class Outer, class... Items>
constexpr std::size_t find_nonunique_member_name(Items...) {
  constexpr std::array names{std::string_view{Items::inner_name()}...};
  constexpr std::array is_member{bool{Items::is_member}...};

  for(std::size_t i=0; i<sizeof...(Items); ++i) {// no constexpr algorithms :-(
    if(!is_member[i]) continue;
    auto count = (std::size_t{0} + ... + (names[i] == Items::inner_name()));
    if(count != 1) return i;
  }
  return sizeof...(Items);
}

template<class Outer, class... Items>
constexpr bool bases_then_members(Items...) {
  bool bases_are_allowed = true;
  for(bool is_base : {bool{Items::is_base}...}) {// is_partitioned
    bases_are_allowed &= is_base;
    if(bases_are_allowed != is_base) return false;
  }
  return true;
}

template<class Outer, class... Items>
constexpr auto has_annotation_of_indirect_base(Items...) {
  constexpr bool ret = (... || ([] (auto base) {
    using Base = decltype(base);
    auto number_of_annotations_that_refer_to_bases_of_this_annotation = (
      std::size_t{0} + ... + (
	std::is_base_of_v<typename Base::inner_type, typename Items::inner_type>
	&& Base::is_base
	&& Items::is_base
      )
    );
    return (number_of_annotations_that_refer_to_bases_of_this_annotation >= 2);
  }(Items{})));

  return std::bool_constant<ret>{};
}

template<class Outer, class... Items>
constexpr bool assert_item_validity(Items...) {
  constexpr std::size_t item_count = sizeof...(Items);

  static_assert(
    panic_nonunique_member_name<Outer, item_count>::template
    in_case< find_nonunique_member_name<Outer>(Items{}...) >()
  );
  static_assert(
    panic_base_after_member<Outer>::template
    unless< bases_then_members<Outer>(Items{}...) >()
  );
  static_assert(
    panic_annotation_of_indirect_base<Outer>
    ::in_case( has_annotation_of_indirect_base<Outer>(Items{}...) )
  );
  return true;
}

template<class Outer, std::size_t... inds>
constexpr bool assert_item_validity(std::index_sequence<inds...>) {
  return assert_item_validity<Outer>(item_impl_t<inds, Outer>{}...);
}

template<class Outer>
constexpr bool assert_item_validity(std::index_sequence</*empty*/>) {
  return true;
}

template<class Outer>
constexpr bool assert_item_validity() {
  constexpr std::size_t size = annotation_tuple<Outer>::size;
  constexpr auto ind_seq = std::make_index_sequence<size>{};
  return assert_item_validity<Outer>(ind_seq);
}

////////////////////////////////////////////////////////////////////////////////

}// ecrypa::detail
