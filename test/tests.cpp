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

TEST_CASE("Parse String path corner cases", "Uri")
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

TEST_CASE("Parse String has a port number", "Uri")
{
  Uri::Uri uri;

  REQUIRE(uri.ParseFromString("https://www.example.com:8080/foo/bar"));
  REQUIRE("www.example.com" == uri.GetHost());
  REQUIRE(uri.HasPort());
  REQUIRE(8080 == uri.GetPort());
}

TEST_CASE("Parse String Twice first has a port number second not", "Uri")
{
  Uri::Uri uri;

  REQUIRE(uri.ParseFromString("https://www.example.com:8080/foo/bar"));
  REQUIRE(uri.ParseFromString("https://www.example.com/foo/bar"));
  REQUIRE(!uri.HasPort());
}

TEST_CASE("Parse String with bad port number all letters", "Uri")
{
  Uri::Uri uri;

  REQUIRE(!uri.ParseFromString("https://www.example.com:false/foo/bar"));
}

TEST_CASE("Parse String with bad port number some letters", "Uri")
{
  Uri::Uri uri;

  REQUIRE(!uri.ParseFromString("https://www.example.com:8080spam/foo/bar"));
}


TEST_CASE("Parse String with huge bad port number", "Uri")
{
  Uri::Uri uri;

  REQUIRE(uri.ParseFromString("https://www.example.com:65535/foo/bar"));
  REQUIRE(65535 == uri.GetPort());

  REQUIRE(!uri.ParseFromString("https://www.example.com:65536/foo/bar"));
}

TEST_CASE("Parse String with negative port", "Uri")
{
  Uri::Uri uri;

  REQUIRE(!uri.ParseFromString("https://www.example.com:-8080/foo/bar"));
}
