#include "percent_encoded_character_decoder.hpp"
#include "character_set.hpp"

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
      impl_->decode_state = second_digit_hex;
    if (DIGITS.Contains(character)) {
      impl_->decoded_character = character - '0';
      break;
    } else if (CharacterSet('a', 'f').Contains(character)) {
      impl_->decoded_character += character - 'a' + LETTER_DISPLACEMENT;
      break;
    } else if (CharacterSet('A', 'F').Contains(character)) {
      impl_->decoded_character += character - 'A' + LETTER_DISPLACEMENT;
      break;
    }
    return false;

  case second_digit_hex:
    impl_->decode_state = done;
    impl_->decoded_character *= HEX_DISPLACEMENT;
    if (DIGITS.Contains(character)) {
      impl_->decoded_character += character - '0';
      break;
    } else if (CharacterSet('a', 'f').Contains(character)) {
      impl_->decoded_character += character - 'a' + LETTER_DISPLACEMENT;
      break;
    } else if (CharacterSet('A', 'F').Contains(character)) {
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
