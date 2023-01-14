#include "../Uri/headers/uri.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Parse String base case", "Uri")
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
    { "foo/", { "" } },
  };

  for (const auto &testVector : testVectors) {

    Uri::Uri uri;

    INFO(testVector.path_in);
    REQUIRE(uri.ParseFromString(testVector.path_in));
    REQUIRE(testVector.path_out == uri.GetPath());
  }
}

TEST_CASE("Parse String with no scheme but with host", "Uri")
{
  Uri::Uri uri;

  REQUIRE(uri.ParseFromString("www.example.com/foo/bar"));
  REQUIRE("www.example.com" == uri.GetHost());
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

TEST_CASE("Parse String that ends after authority", "Uri")
{
  Uri::Uri uri;

  REQUIRE(uri.ParseFromString("https://www.example.com"));
}

TEST_CASE("Parse String relative and non relative references", "Uri")
{
  struct TestVector
  {
    std::string path_in;
    bool is_relative;
  };

  const std::vector<TestVector> testVectors{
    { "https://www.example.com/", false },
    { "https://www.example.com", false },
    { "foo", true },
    { "/", true },
  };

  for (const auto &testVector : testVectors) {

    Uri::Uri uri;

    REQUIRE(uri.ParseFromString(testVector.path_in));
    REQUIRE(testVector.is_relative == uri.IsRelativeReference());
  }
}

TEST_CASE("Parse String relative and non relative path", "Uri")
{
  struct TestVector
  {
    std::string path_in;
    bool is_relative;
  };

  const std::vector<TestVector> testVectors{
    { "https://www.example.com/", false },
    { "https://www.example.com", true },
    { "foo", true },
    { "/", false },
  };

  for (const auto &testVector : testVectors) {

    Uri::Uri uri;

    REQUIRE(uri.ParseFromString(testVector.path_in));
    REQUIRE(testVector.is_relative == uri.IsRelativePath());
  }
}

TEST_CASE("Parse String with query and fragment", "Uri")
{
  struct TestVector
  {
    std::string path_in;
    std::string host;
    std::string query;
    std::string fragment;
  };

  const std::vector<TestVector> testVectors{
    { "https://www.example.com/", "www.example.com", "", "" },
    { "https://example.com?foo", "example.com", "foo", "" },
    { "https://example.com#foo", "example.com", "", "foo" },
    { "https://www.example.com?foo#bar", "www.example.com", "foo", "bar" },
    { "https://www.example.com?foo?earth#bar",
      "www.example.com",
      "foo?earth",
      "bar" },
    { "https://www.example.com/spam?foo#bar", "www.example.com", "foo", "bar" },
  };

  for (const auto &testVector : testVectors) {

    Uri::Uri uri;
    INFO(testVector.path_in);
    REQUIRE(uri.ParseFromString(testVector.path_in));
    REQUIRE(testVector.host == uri.GetHost());
    REQUIRE(testVector.query == uri.GetQuery());
    REQUIRE(testVector.fragment == uri.GetFragment());
  }
}

TEST_CASE("Parse String with user info ", "Uri")
{
  struct TestVector
  {
    std::string path_in;
    std::string username;
  };

  const std::vector<TestVector> testVectors{
    { "https://www.example.com/", "" },
    { "https://joe@www.example.com", "joe" },
    // { "joe@www.example.com:8080", "joe" },
    // { "https://joe@www.example.com:8080", "joe" },
  };

  for (const auto &testVector : testVectors) {
    Uri::Uri uri;

    INFO(testVector.path_in);
    REQUIRE(uri.ParseFromString(testVector.path_in));
    REQUIRE(testVector.username == uri.GetUserName());
  }
}