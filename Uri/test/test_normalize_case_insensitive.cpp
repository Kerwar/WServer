#include <catch2/catch.hpp>

#include "../src/normalize_case_insensitive_string.hpp"

TEST_CASE("Normalize case insentice string", "[NormalizeCaseInsensitiveString]")
{
  REQUIRE("example" == Uri::NormalizeCaseInsensitiveString("example"));
  REQUIRE("example" == Uri::NormalizeCaseInsensitiveString("EXAMPLE"));
  REQUIRE("example" == Uri::NormalizeCaseInsensitiveString("exAMPle"));
  REQUIRE("example" == Uri::NormalizeCaseInsensitiveString("ExamplE"));
  REQUIRE("foo1bar" == Uri::NormalizeCaseInsensitiveString("foo1bar"));
  REQUIRE("foo1bar" == Uri::NormalizeCaseInsensitiveString("FOO1BAR"));
}
