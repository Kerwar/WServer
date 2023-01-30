#include "character_set.hpp"
#include <set>

namespace Uri {

struct CharacterSet::Implementation
{
  std::set<char> character_in_set;
};

CharacterSet::~CharacterSet() noexcept = default;
CharacterSet::CharacterSet(const CharacterSet &other) : impl_(new Implementation(*other.impl_)) {}
CharacterSet::CharacterSet(CharacterSet &&) noexcept = default;
CharacterSet &CharacterSet::operator=(const CharacterSet &other)
{
  if (this != &other) { *impl_ = *other.impl_; }
  return *this;
}
CharacterSet &CharacterSet::operator=(CharacterSet &&other) noexcept = default;

CharacterSet::CharacterSet() : impl_(new Implementation) {}


CharacterSet::CharacterSet(char character) : impl_(new Implementation)
{
  impl_->character_in_set.insert(character);
}

CharacterSet::CharacterSet(char first, char last) : impl_(new Implementation)
{
  if (first > last) { std::swap(first, last); }
  for (char character = first; character < last + 1; character++) {
    impl_->character_in_set.insert(character);
  }
}

CharacterSet::CharacterSet(std::initializer_list<const CharacterSet> character_sets)
  : impl_(new Implementation)
{
  for (auto set : character_sets) {
    impl_->character_in_set.insert(
      set.impl_->character_in_set.begin(), set.impl_->character_in_set.end());
  }
}

bool CharacterSet::Contains(char character) const
{
  return impl_->character_in_set.find(character) != impl_->character_in_set.end();
}

}// namespace Uri
