#include <iostream>

namespace project {

// used as base class
class Animal {
 private:
  int age_ = 0;

  template<class Os> friend Os& operator<<(Os& os, const Animal& self)
  { return os << "Animal{age: " << self.age_ << "}"; }

// ... omitted as offtopic: [treat all malicious side-effects of inheritance]...
};

// referenced by reference member
long global_family_size = 0;

// class that annotates its internals
class Dog
  : public Animal
{
 private:
  int teeth_count_ = 42;
  int leg_count_ = 4;
  double max_speed_km_per_h_ = 42.5;

  long& family_size_ = ++global_family_size;

  template<class A> friend constexpr auto annotate(A a, Dog*) {
    return a(
      // annotate base classes
      (Animal*){},

      // annotate non-reference members
      &Dog::teeth_count_,
      &Dog::leg_count_,
      &Dog::max_speed_km_per_h_,

      // annotate reference members (yeah, it's ugly...)
      a([] (auto self) { return A::lref(self->family_size_); }, "family_size_")
    );
  }
};

}// project

// The annotation above is free of dependencies!
#include <ecrypa/ecrypa.hpp>

int main() {
  project::Dog dog{};

  std::cout << "=== bases ===" << std::endl;
  ecrypa::each_base<project::Dog>([&] (auto item) {
    std::cout << item.inner_type_name() << " "
	      << item.outer_type_name() << "::"
	      << item.inner_name() << " = "
	      << item(dog)
	      << std::endl;
  });
  std::cout << std::endl;

  std::cout << "=== members ===" << std::endl;
  ecrypa::each_member<project::Dog>([&] (auto item) {
    std::cout << item.inner_type_name() << " "
	      << item.outer_type_name() << "::"
	      << item.inner_name() << " = "
	      << item(dog)
	      << std::endl;
  });
  std::cout << std::endl;

  // TODO: Design a good interface...
  constexpr auto first_member_handle = ecrypa::item<1, project::Dog>{};
  --first_member_handle(dog);

  std::cout << "=== lost one tooth ===" << std::endl;
  ecrypa::each_item<project::Dog>([&] (auto item) {
    std::cout << item.inner_type_name() << " "
	      << item.outer_type_name() << "::"
	      << item.inner_name() << " = "
	      << item(dog)
	      << std::endl;
  });
}
