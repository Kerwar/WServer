#include <catch2/catch.hpp>

#include "../src/percent_encoded_character_decoder.hpp"

TEST_CASE("Good sequence of encoded characters", "[PercentEncodedCharacterDecoder]")
{
  struct TestVector
  {
    std::array<char, 2> characters;
    char decoded_character;
  };

  std::vector<TestVector> test_vectors{
    { { '4', '1' }, 'A' },
    { { '5', 'A' }, 'Z' },
    { { '6', 'e' }, 'n' },
  };

  for (auto test_vector : test_vectors) {
    Uri::PercentEncodedCharacterDecoder percent_decoder;

    REQUIRE(percent_decoder.NextEncodedCharacter(test_vector.characters[0]));
    REQUIRE(percent_decoder.NextEncodedCharacter(test_vector.characters[1]));
    REQUIRE(percent_decoder.Done());
    REQUIRE(test_vector.decoded_character == percent_decoder.GetDecodedCharacter());
  }
}

TEST_CASE("Bad sequence of encoded characters", "[PercentEncodedCharacterDecoder]")
{
  std::vector<char> test_vectors{
    'G',
    'g',
    '%',
    ',',
    '-',
    'V',
    'z',
  };

  for (auto test_vector : test_vectors) {
    Uri::PercentEncodedCharacterDecoder percent_decoder;

    REQUIRE_FALSE(percent_decoder.Done());
    REQUIRE_FALSE(percent_decoder.NextEncodedCharacter(test_vector));
  }
}

