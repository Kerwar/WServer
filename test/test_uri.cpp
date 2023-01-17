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
    { "foo/", { "foo", "" } },
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

  REQUIRE(uri.ParseFromString("//www.example.com/foo/bar"));
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
    { "/?foo#bar", "", "foo", "bar" },
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
    { "//joe@www.example.com:8080", "joe" },
    // { "https://joe@www.example.com:8080", "joe" },
  };

  for (const auto &testVector : testVectors) {
    Uri::Uri uri;

    INFO(testVector.path_in);
    REQUIRE(uri.ParseFromString(testVector.path_in));
    REQUIRE(testVector.username == uri.GetUserName());
  }
}

TEST_CASE("Parse String with scheme corner cases", "Uri")
{
  struct TestVector
  {
    std::string path_in;
    std::string scheme;
  };

  const std::vector<TestVector> testVectors{
    { "h://www.example.com/", "h" },
    { "x+://www.example.com/", "x+" },
    { "x-://www.example.com/", "x-" },
    { "z.://www.example.com/", "z." },
    { "aa://www.example.com/", "aa" },
    { "a0://www.example.com/", "a0" },
  };

  for (const auto &testVector : testVectors) {
    Uri::Uri uri;

    INFO(testVector.path_in);
    REQUIRE(uri.ParseFromString(testVector.path_in));
    REQUIRE(testVector.scheme == uri.GetScheme());
  }
}


TEST_CASE("Parse String for bad scheme with illegal characters", "Uri")
{
  const std::vector<std::string> test_uris{
    "@://www.example.com/",
    "0://www.example.com/",
    "+://www.example.com/",
    "-://www.example.com/",
    "$://www.example.com/",
    "1://www.example.com/",
    "h@://www.example.com/",
    "h~://www.example.com/",
  };

  for (const auto &test_uri : test_uris) {

    Uri::Uri uri;

    REQUIRE_FALSE(uri.ParseFromString(test_uri));
  }
}

TEST_CASE("Parse String for bad usernames because illegal characters", "Uri")
{
  const std::vector<std::string> test_uris{
    "//%X@www.example.com/",
    "//%F5@www.example.com/",
    "//%5@www.example.com/",
    "//%5G@www.example.com/",
    "//{@www.example.com/",
  };

  for (const auto &test_uri : test_uris) {

    Uri::Uri uri;

    INFO("Path in: " + test_uri);
    REQUIRE_FALSE(uri.ParseFromString(test_uri));
  }
}

TEST_CASE("Parse String with user name with corner cases", "Uri")
{
  struct TestVector
  {
    std::string path_in;
    std::string user_name;
  };

  const std::vector<TestVector> testVectors{
    { "//%41@www.example.com/", "A" },
    { "//@www.example.com/", "" },
    { "//!@www.example.com/", "!" },
    { "//'@www.example.com/", "'" },
    { "//(@www.example.com/", "(" },
    { "//;@www.example.com/", ";" },
    { "//:@www.example.com/", ":" },
  };

  for (const auto &testVector : testVectors) {
    Uri::Uri uri;

    INFO(testVector.path_in);
    REQUIRE(uri.ParseFromString(testVector.path_in));
    REQUIRE(testVector.user_name == uri.GetUserName());
  }
}

TEST_CASE("Parse String for bad hosts", "Uri")
{
  const std::vector<std::string> test_uris{
    "//@www:example.com/",
    "//[vX.:]/",
  };

  for (const auto &test_uri : test_uris) {

    Uri::Uri uri;

    INFO("Path in: " + test_uri);
    REQUIRE_FALSE(uri.ParseFromString(test_uri));
  }
}


TEST_CASE("Parse String with host corner cases", "Uri")
{
  struct TestVector
  {
    std::string path_in;
    std::string host;
  };

  const std::vector<TestVector> testVectors{
    { "//%41/", "A" },
    { "///", "" },
    { "//!/", "!" },
    { "//'/", "'" },
    { "//(/", "(" },
    { "//;/", ";" },
    { "//1.2.3.4/", "1.2.3.4" },
    { "//[v7.:]/", "[v7.:]" },
  };

  for (const auto &testVector : testVectors) {
    Uri::Uri uri;

    INFO("Path in: " + testVector.path_in);
    REQUIRE(uri.ParseFromString(testVector.path_in));
    REQUIRE(testVector.host == uri.GetHost());
  }
}

TEST_CASE("Parse String missinterpret colon", "Uri")
{

  const std::vector<std::string> test_uris{
    "//foo:bar@www.example.com/",
    "//www.example.com/a:b",
    "//www.example.com/foo?a:b",
    "//www.example.com/foo#a:b",
    "//[v7.:]/",
  };

  for (const auto &test_uri : test_uris) {

    Uri::Uri uri;

    INFO("Path in: " + test_uri);
    REQUIRE(uri.ParseFromString(test_uri));
  }
}

TEST_CASE("Parse String for bad path", "Uri")
{
  const std::vector<std::string> test_uris{
    "//www.example.com/foo[bar",
    "//www.example.com/]bar",
    "//www.example.com/foo[",
    "//www.example.com/]",
    "//www.example.com/foo[bar",
    "//www.example.com/foo/[bar",
    "//www.example.com/foo[bar/",
    "//www.example.com/foo/[/",
    "/foo[bar",
    "/]bar",
    "/foo[",
    "/]",
    "/foo[bar",
    "/foo/[bar",
    "/foo[bar/",
    "/foo/[/",
  };

  for (const auto &test_uri : test_uris) {

    Uri::Uri uri;

    INFO("Path in: " + test_uri);
    REQUIRE_FALSE(uri.ParseFromString(test_uri));
  }
}

TEST_CASE("Parse String with path corner cases", "Uri")
{
  struct TestVector
  {
    std::string path_in;
    std::vector<std::string> path;
  };

  const std::vector<TestVector> testVectors{
    { "/:/foo", { "", ":", "foo" } },
    { "bob@/foo", { "bob@", "foo" } },
    { "hello!", { "hello!" } },
    { "urn:hello,%20w%6Frld", { "hello, world" } },
    { "//example.xom/foo/(bar", { "", "foo", "(bar" } },
  };

  for (const auto &testVector : testVectors) {
    Uri::Uri uri;

    INFO("Path in: " + testVector.path_in);
    REQUIRE(uri.ParseFromString(testVector.path_in));
    REQUIRE(testVector.path == uri.GetPath());
  }
}

TEST_CASE("Parse String for bad query because illegal characters", "Uri")
{
  const std::vector<std::string> test_uris{
    "//www.example.com?foo[bar",
    "//www.example.com?]bar",
    "//www.example.com?foo[",
    "//www.example.com?]",
    "//www.example.com?foo[bar",
    "//www.example.com?foo/[bar",
    "//www.example.com?foo[bar/",
    "//www.example.com/foo?[/",
    "?foo[bar",
    "?]bar",
    "?foo[",
    "?]",
    "?foo[bar",
    "?foo/[bar",
    "?foo[bar/",
    "/foo?[/",
  };

  for (const auto &test_uri : test_uris) {

    Uri::Uri uri;

    INFO("Path in: " + test_uri);
    REQUIRE_FALSE(uri.ParseFromString(test_uri));
  }
}

TEST_CASE("Parse String with query corner cases", "Uri")
{
  struct TestVector
  {
    std::string path_in;
    std::string query;
  };

  const std::vector<TestVector> testVectors{
    { "/?:/foo", ":/foo" },
    { "?bob@/foo", "bob@/foo" },
    { "?hello!", "hello!" },
    { "urn:?hello,%20w%6Frld", "hello, world" },
    { "//example.com/foo?(bar)", "(bar)" },
    { "//example.com?foo?(bar)", "foo?(bar)" },
  };

  for (const auto &testVector : testVectors) {
    Uri::Uri uri;

    INFO("Path in: " + testVector.path_in);
    REQUIRE(uri.ParseFromString(testVector.path_in));
    REQUIRE(testVector.query == uri.GetQuery());
  }
}
