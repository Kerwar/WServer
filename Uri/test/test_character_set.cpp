#include "../src/character_set.hpp"
#include <catch2/catch.hpp>

const char last_character = static_cast<char>(0x7F);

TEST_CASE("Default constructor", "CharacterSet")
{
  Uri::CharacterSet character_set;

  for (char character = 0; character < last_character; ++character) {
    REQUIRE_FALSE(character_set.Contains(character));
  }
}

TEST_CASE("Single character constructor", "CharacterSet")
{
  Uri::CharacterSet character_set('%');

  for (char character = 0; character < last_character; ++character) {
    if (character == '%') {
      REQUIRE(character_set.Contains(character));
    } else {
      REQUIRE_FALSE(character_set.Contains(character));
    }
  }
}

TEST_CASE("Range constructor", "CharacterSet")
{
  Uri::CharacterSet character_set('A', 'G');

  for (char character = 0; character < last_character; ++character) {
    if (character >= 'A' && character <= 'G') {
      REQUIRE(character_set.Contains(character));
    } else {
      REQUIRE_FALSE(character_set.Contains(character));
    }
  }
}

TEST_CASE("Initializer list constructor", "CharacterSet")
{
  Uri::CharacterSet character_set{ Uri::CharacterSet('A', 'G'), Uri::CharacterSet('f', 'm') };

  for (char character = 0; character < last_character; ++character) {
    if ((character >= 'A' && character <= 'G') || (character >= 'f' && character <= 'm')) {
      REQUIRE(character_set.Contains(character));
    } else {
      REQUIRE_FALSE(character_set.Contains(character));
    }
  }
}

