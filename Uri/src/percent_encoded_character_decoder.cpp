#include "percent_encoded_character_decoder.hpp"
#include "character_in_set.hpp"

namespace Uri {

struct PercentEncodedCharacterDecoder::Implementation
{
  int decoded_character = 0;
  Decode_state decode_state = first_digit_hex;
};

PercentEncodedCharacterDecoder::PercentEncodedCharacterDecoder()
  : impl_(new Implementation)
{}

PercentEncodedCharacterDecoder::~PercentEncodedCharacterDecoder() = default;

PercentEncodedCharacterDecoder::PercentEncodedCharacterDecoder(
  PercentEncodedCharacterDecoder &&) noexcept = default;

PercentEncodedCharacterDecoder &PercentEncodedCharacterDecoder::operator=(
  PercentEncodedCharacterDecoder &&) noexcept = default;

bool PercentEncodedCharacterDecoder::NextEncodedCharacter(char character)
{
  switch (impl_->decode_state) {

  case first_digit_hex:
    if (IsCharacterInSet(character, DIGITS)) {
      impl_->decode_state = second_digit_hex;
      impl_->decoded_character = character - '0';
      break;
    }
    return false;

  case second_digit_hex:
    impl_->decode_state = done;
    impl_->decoded_character *= HEX_DISPLACEMENT;
    if (IsCharacterInSet(character, DIGITS)) {
      impl_->decoded_character += character - '0';
      break;
    } else if (character >= 'A' && character <= 'F') {
      impl_->decoded_character += character - 'A' + LETTER_DISPLACEMENT;
      break;
    }
    return false;

  case done:
    return false;
  }

  return true;
}

bool PercentEncodedCharacterDecoder::Done() const
{
  return impl_->decode_state == done;
}

char PercentEncodedCharacterDecoder::GetDecodedCharacter() const
{
  return static_cast<char>(impl_->decoded_character);
}

}// namespace Uri
