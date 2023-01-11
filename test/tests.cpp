#include "../Uri/headers/uri.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Parse String URL case", "Uri")
{
  Uri::Uri uri;

  REQUIRE(uri.ParseFromString("https://www.example.com/foo/bar"));
  REQUIRE("https" == uri.GetScheme());
  REQUIRE("www.example.com" == uri.GetHost());
  REQUIRE(std::vector<std::string>{
            "",
            "foo",
            "bar",
          }
          == uri.GetPath());
}

TEST_CASE("Parse String URN one character", "Uri")
{
  Uri::Uri uri;
  uri.SetPathDelimiter(":");

  REQUIRE(uri.ParseFromString("urn:book:fantasy:LDM"));
  REQUIRE("urn" == uri.GetScheme());
  REQUIRE(uri.GetHost().empty());
  REQUIRE(std::vector<std::string>{
            "book",
            "fantasy",
            "LDM",
          }
          == uri.GetPath());
}

TEST_CASE("Parse String URN multicharacter path delimiter", "Uri")
{
  Uri::Uri uri;
  uri.SetPathDelimiter("/-");

  REQUIRE(uri.ParseFromString("urn:bo-/ok/-fant/asy-LD-M"));
  REQUIRE("urn" == uri.GetScheme());
  REQUIRE(uri.GetHost().empty());
  REQUIRE(std::vector<std::string>{
            "bo-/ok",
            "fant/asy",
            "LD-M",
          }
          == uri.GetPath());
}
