#include "percent_encoded_character_decoder.hpp"
#include "character_set.hpp"

namespace Uri {

struct PercentEncodedCharacterDecoder::Implementation
{
  int decoded_character = 0;
  int digits_left = 2;

  bool ShiftHexDigit(char character)
  {
    decoded_character *= HEX_DISPLACEMENT;
    if (DIGITS.Contains(character)) {
      decoded_character += character - '0';
      return true;
    } else if (CharacterSet('a', 'f').Contains(character)) {
      decoded_character += character - 'a' + LETTER_DISPLACEMENT;
      return true;
    } else if (CharacterSet('A', 'F').Contains(character)) {
      decoded_character += character - 'A' + LETTER_DISPLACEMENT;
      return true;
    }

    return false;
  }
};

PercentEncodedCharacterDecoder::PercentEncodedCharacterDecoder() : impl_(new Implementation) {}

PercentEncodedCharacterDecoder::~PercentEncodedCharacterDecoder() = default;

PercentEncodedCharacterDecoder::PercentEncodedCharacterDecoder(
  PercentEncodedCharacterDecoder &&) noexcept = default;

PercentEncodedCharacterDecoder &PercentEncodedCharacterDecoder::operator=(
  PercentEncodedCharacterDecoder &&) noexcept = default;

bool PercentEncodedCharacterDecoder::NextEncodedCharacter(char character)
{
  if (impl_->digits_left > 0) {
    --impl_->digits_left;
    return impl_->ShiftHexDigit(character);
  }
  return false;
}

bool PercentEncodedCharacterDecoder::Done() const { return impl_->digits_left == 0; }

char PercentEncodedCharacterDecoder::GetDecodedCharacter() const
{
  return static_cast<char>(impl_->decoded_character);
}


}// namespace Uri
