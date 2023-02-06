#ifndef URI_PERCENT_ENCODED_CHARACTER_DECODER_HPP
#define URI_PERCENT_ENCODED_CHARACTER_DECODER_HPP

#include <memory>

namespace Uri {

class PercentEncodedCharacterDecoder
{
public:
  ~PercentEncodedCharacterDecoder();
  PercentEncodedCharacterDecoder(const PercentEncodedCharacterDecoder &) = delete;
  PercentEncodedCharacterDecoder(PercentEncodedCharacterDecoder &&) noexcept;
  PercentEncodedCharacterDecoder &operator=(const PercentEncodedCharacterDecoder &) = delete;
  PercentEncodedCharacterDecoder &operator=(PercentEncodedCharacterDecoder &&) noexcept;

  PercentEncodedCharacterDecoder();
  /* This method inputs the next encoded character-
   *
   * @param [in] character
   *    This is the next encoded character to give to the decoded_character
   *
   *    @return An indication of wheter or not the encoded character
   *    was accepted is returned
   */
  bool NextEncodedCharacter(char character);

  /* This method checks if the decoder is done
   *
   * @param [in] character
   *    This is the next encoded character to give to the decoded_character
   *
   *    @return An indication of wheter or not the encoded character
   *    was accepted is returned
   */
  [[nodiscard]] bool Done() const;

  /* This method returns the decoded character once the decoder is done
   *
   * @return
   *     The decoded character
   */
  [[nodiscard]] char GetDecodedCharacter() const;

private:
  const static int LETTER_DISPLACEMENT = 10;
  const static int HEX_DISPLACEMENT = 16;
  enum Decode_state { first_digit_hex, second_digit_hex, done };
  struct Implementation;

  std::unique_ptr<Implementation> impl_;
};

}// namespace Uri

#endif// !URI_PERCENT_ENCODED_CHARACTER_DECODER_HPP
