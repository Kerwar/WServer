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

  REQUIRE(uri.ParseFromString("urn:bo-/ok/-fant/asy/-LD-M"));
  REQUIRE("urn" == uri.GetScheme());
  REQUIRE(uri.GetHost().empty());
  REQUIRE(std::vector<std::string>{
            "bo-/ok",
            "fant/asy",
            "LD-M",
          }
          == uri.GetPath());
}

TEST_CASE("Parse String URN path corner cases", "Uri")
{
  struct TestVector
  {
    std::string path_in;
    std::vector<std::string> path_out;
  };

  const std::vector<TestVector> testVectors{
    { "", {} },
    { "/", { "" } },
    { "/foo", { "", "foo" } },
    { "foo/", { "foo", "" } },
  };

  for (const auto &testVector : testVectors) {

    Uri::Uri uri;

    REQUIRE(uri.ParseFromString(testVector.path_in));
    REQUIRE(testVector.path_out == uri.GetPath());
  }
}
