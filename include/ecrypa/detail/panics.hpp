#pragma once

#include <cstddef>

#include <type_traits>

namespace ecrypa::detail {

////////////////////////////////////////////////////////////////////////////////

template<class Outer>
struct ECRYPA_PANIC_ANNOTATION_NOT_FOUND;

template<class Outer>
struct panic_annotation_not_found {
  template<class IsAnnotated>
  static constexpr bool unless(IsAnnotated) {
    static_assert(IsAnnotated{}, R"!!!(ECRYPA APOLOGIZES

 ###############################################################################################################################################################################################################################################################################################################################################################################################################

 For this class, ecrypa was not able to find a valid `annotate`
 function.

 Besides this message, the compiler should emit diagnoses related to
 `ECRYPA_PANIC_ANNOTATION_NOT_FOUND`. Those shall help you identifying
 the problem.

 ###############################################################################################################################################################################################################################################################################################################################################################################################################)!!!");
    if constexpr(!IsAnnotated{}) {
      static_assert(ECRYPA_PANIC_ANNOTATION_NOT_FOUND<Outer>{});
    }
    return IsAnnotated{};
  }
};

////////////////////////////////////////////////////////////////////////////////

template<class Derived, class Base>
struct ECRYPA_PANIC_ANNOTATION_OF_AMBIGUOUS_BASE;

template<class Derived, class Base>
struct panic_annotation_of_ambiguous_base {
  template<class IsAmbiguousBase>
  static constexpr bool in_case(IsAmbiguousBase) {
    static_assert(!IsAmbiguousBase{}, R"!!!(ECRYPA APOLOGIZES

 ###############################################################################################################################################################################################################################################################################################################################################################################################################

 This base annotation seems to annotate an _ambiguous_ base class.

 Besides this message, the compiler should emit diagnoses related to
 `ECRYPA_PANIC_ANNOTATION_OF_AMBIGUOUS_BASE`. Those shall help you
 identifying the problematic annotation.

 ###############################################################################################################################################################################################################################################################################################################################################################################################################)!!!");
    if constexpr(IsAmbiguousBase{}) {
      static_assert(ECRYPA_PANIC_ANNOTATION_OF_AMBIGUOUS_BASE<Derived, Base>{});
    }
    return !IsAmbiguousBase{};
  }
};

////////////////////////////////////////////////////////////////////////////////

template<class Derived>
struct ECRYPA_PANIC_ANNOTATION_OF_INDIRECT_BASE;

template<class Derived>
struct panic_annotation_of_indirect_base {
  template<class HasAnnotationOfIndirectBase>
  static constexpr bool in_case(HasAnnotationOfIndirectBase) {
    static_assert(!HasAnnotationOfIndirectBase{}, R"!!!(ECRYPA APOLOGIZES

 ###############################################################################################################################################################################################################################################################################################################################################################################################################

 There seems to exist an annotation of an _indirect_ base.

 Besides this message, the compiler should emit diagnoses related to
 `ECRYPA_PANIC_ANNOTATION_OF_INDIRECT_BASE`. Those shall help you
 identifying the problematic annotation.

 ###############################################################################################################################################################################################################################################################################################################################################################################################################)!!!");
    if constexpr(HasAnnotationOfIndirectBase{}) {
      static_assert(ECRYPA_PANIC_ANNOTATION_OF_INDIRECT_BASE<Derived>{});
    }
    return !HasAnnotationOfIndirectBase{};
  }
};

////////////////////////////////////////////////////////////////////////////////

template<class Outer>
struct ECRYPA_PANIC_BASE_AFTER_MEMBER;

template<class Outer>
struct panic_base_after_member {
  template<bool bases_then_members>
  static constexpr bool unless() {
    static_assert(bases_then_members, R"!!!(ECRYPA APOLOGIZES

 ###############################################################################################################################################################################################################################################################################################################################################################################################################

 Ecrypa identified a base annotation after a member annotation. That
 is illegal. The correct order is: bases first; members last.

 Besides this message, the compiler should emit diagnoses related to
 `ECRYPA_PANIC_BASE_AFTER_MEMBER`. Those shall help you identifying
 the problem.

 ###############################################################################################################################################################################################################################################################################################################################################################################################################)!!!");
    if constexpr(!bases_then_members) {
      static_assert(ECRYPA_PANIC_BASE_AFTER_MEMBER<Outer>{});
    }
    return bases_then_members;
  }
};

////////////////////////////////////////////////////////////////////////////////

template<std::size_t idx_requested, class Outer, std::size_t size_avail>
struct ECRYPA_PANIC_IDX_OUT_OF_RANGE;

template<std::size_t idx_requested, class Outer, std::size_t size_avail>
struct panic_idx_out_of_range {
  template<bool idx_is_valid>
  static constexpr bool unless() {
    static_assert(idx_is_valid, R"!!!(ECRYPA APOLOGIZES

 ###############################################################################################################################################################################################################################################################################################################################################################################################################

 Attempt to access an out-of-range annotation.

 Besides this message, the compiler should emit diagnoses related to
 `ECRYPA_PANIC_IDX_OUT_OF_RANGE`. Those shall help you identifying the
 problem.

 ###############################################################################################################################################################################################################################################################################################################################################################################################################)!!!");
    if constexpr(!idx_is_valid) {
      static_assert(
	ECRYPA_PANIC_IDX_OUT_OF_RANGE<idx_requested, Outer, size_avail>{}
      );
    }
    return idx_is_valid;
  }
};

////////////////////////////////////////////////////////////////////////////////

template<class Outer, std::size_t problematic_item_idx>
struct ECRYPA_PANIC_NONUNIQUE_MEMBER_NAME;

template<class Outer, std::size_t item_count>
struct panic_nonunique_member_name {
  template<std::size_t problematic_item_idx>
  static constexpr bool in_case() {
    static_assert(problematic_item_idx == item_count, R"!!!(ECRYPA APOLOGIZES

 ###############################################################################################################################################################################################################################################################################################################################################################################################################

 There are two member annotations with the same name.

 Besides this message, the compiler should emit diagnoses related to
 `ECRYPA_PANIC_NONUNIQUE_MEMBER_NAME`. Those shall help you
 identifying the problem.

 ###############################################################################################################################################################################################################################################################################################################################################################################################################)!!!");
    if constexpr(problematic_item_idx != item_count) {
      static_assert(
	ECRYPA_PANIC_NONUNIQUE_MEMBER_NAME<Outer, problematic_item_idx>{}
      );
    }
    return (problematic_item_idx == item_count);
  }
};

////////////////////////////////////////////////////////////////////////////////

template<class Outer, class UnknownAnnotation>
struct ECRYPA_PANIC_UNKNOWN_ANNOTATION;

template<class Outer, class UnknownAnnotation>
struct panic_unknown_annotation {
  template<class False = std::false_type>
  static constexpr bool because_item_impl_needs_specialization(False = {}) {
    static_assert(False{}, R"!!!(ECRYPA APOLOGIZES

 ###############################################################################################################################################################################################################################################################################################################################################################################################################

 This annotation can not be handled by ecrypa.

 Besides this message, the compiler should emit diagnoses related to
 `ECRYPA_PANIC_UNKNOWN_ANNOTATION`. Those shall help you identifying
 the problematic annotation.

 ###############################################################################################################################################################################################################################################################################################################################################################################################################)!!!");
    if constexpr(false == False{}) {
      static_assert(ECRYPA_PANIC_UNKNOWN_ANNOTATION<Outer, UnknownAnnotation>{});
    }
    return False{};
  }
};

////////////////////////////////////////////////////////////////////////////////

}// ecrypa::detail
