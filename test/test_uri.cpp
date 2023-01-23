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

    INFO("Path in: " + testVector.path_in);
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
    { "https://www.example.com", false },
    { "foo", true },
    { "/", false },
  };

  for (const auto &testVector : testVectors) {

    Uri::Uri uri;

    INFO("Path in: " + testVector.path_in);
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

TEST_CASE("Parse String with scheme in lowercase and uppercase", "Uri")
{
  const std::vector<std::string> testVectors{
    { "http://www.example.com/" },
    { "hTtp://www.example.com/" },
    { "HTTP://www.example.com/" },
    { "Http://www.example.com/" },
    { "HttP://www.example.com/" },
  };

  for (const auto &testVector : testVectors) {
    Uri::Uri uri;

    INFO(testVector);
    REQUIRE(uri.ParseFromString(testVector));
    REQUIRE("http" == uri.GetScheme());
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
    "//[vF.u]r/",
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
    { "//%41/", "a" },
    { "///", "" },
    { "//!/", "!" },
    { "//'/", "'" },
    { "//(/", "(" },
    { "//;/", ";" },
    { "//1.2.3.4/", "1.2.3.4" },
    { "//[v7.:]/", "[v7.:]" },
    { "//[v7.aB]/", "[v7.aB]" },
  };

  for (const auto &testVector : testVectors) {
    Uri::Uri uri;

    INFO("Path in: " + testVector.path_in);
    REQUIRE(uri.ParseFromString(testVector.path_in));
    REQUIRE(testVector.host == uri.GetHost());
  }
}

TEST_CASE("Parse String with hots in lowercase and uppercase", "Uri")
{
  const std::vector<std::string> testVectors{
    { "http://www.example.com/" },
    { "http://www.EXAMPLE.com/" },
    { "HTTP://www.example.COM/" },
    { "Http://www.exAMple.cOM/" },
    { "HttP://wWw.ExAmpLe.com/" },
  };

  for (const auto &testVector : testVectors) {
    Uri::Uri uri;

    INFO(testVector);
    REQUIRE(uri.ParseFromString(testVector));
    REQUIRE("www.example.com" == uri.GetHost());
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
    { "bob@/foo/", { "bob@", "foo", "" } },
    { "hello!", { "hello!" } },
    { "urn:hello,%20w%6Frld", { "hello, world" } },
    { "//example.xom/foo/(bar", { "", "foo", "(bar" } },
    { "?query", {} },
    { "https://a/", { "" } },
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

TEST_CASE("Parse String with percent encode characters", "Uri")
{
  struct TestVector
  {
    std::string path_in;
    std::string path_first_segment;
  };

  const std::vector<TestVector> testVectors{
    { "%41", "A" },
    { "%4A", "J" },
    { "%4a", "J" },
    { "%bc", "\xbc" },
    { "%Bc", "\xbc" },
    { "%bC", "\xbc" },
    { "%BC", "\xbc" },
    { "%41%42%43", "ABC" },
    { "%41%4A%43%4b", "AJCK" },
  };

  for (const auto &testVector : testVectors) {
    Uri::Uri uri;

    INFO("Path in: " + testVector.path_in);
    REQUIRE(uri.ParseFromString(testVector.path_in));
    REQUIRE(testVector.path_first_segment == uri.GetPath()[0]);
  }
}

TEST_CASE("Normalize path", "Uri")
{
  struct TestVector
  {
    std::string path_in;
    std::vector<std::string> path;
  };

  const std::vector<TestVector> testVectors{
    { "/a/b/c/./../../g", { "", "a", "g" } },
    { "mid/constent=5/../6", { "mid", "6" } },
    { "../mid/constent=5/../6", { "mid", "6" } },
    { "https://www.example.com/a/../b", { "", "b" } },
    { "https://www.example.com/a/../../b", { "", "b" } },
    { "./a/b", { "a", "b" } },
    { "..", {} },
    { "a/b/..", { "a", "" } },
    { "a/b/.", { "a", "b", "" } },
    { "a/b/./c", { "a", "b", "c" } },
    { "a/b/./c/", { "a", "b", "c", "" } },
    { "/a/b/..", { "", "a", "" } },
    { "/a/b/.", { "", "a", "b", "" } },
    { "/a/b/./c", { "", "a", "b", "c" } },
    { "/a/b/./c/", { "", "a", "b", "c", "" } },
    { "../a/b/..", { "a", "" } },
    { "../a/b/.", { "a", "b", "" } },
    { "../a/b/./c", { "a", "b", "c" } },
    { "../a/b/./c/", { "a", "b", "c", "" } },
    { "/./a/b/../c/", { "", "a", "c", "" } },
    { "/../a/b/../c/", { "", "a", "c", "" } },
    { "../../", { "" } },
    { "../..", {  } },
  };

  for (const auto &testVector : testVectors) {
    Uri::Uri uri;

    INFO("Path in: " + testVector.path_in);
    REQUIRE(uri.ParseFromString(testVector.path_in));
    uri.NormalizePath();
    REQUIRE(testVector.path == uri.GetPath());
  }
}

TEST_CASE("Normalization and equivalent uri", "Uri")
{
  Uri::Uri uri1;
  Uri::Uri uri2;

  REQUIRE(uri1.ParseFromString("example://a/b/c/%7Bfoo%7D"));
  REQUIRE(uri2.ParseFromString("eXAMPLe://a/./b/../b/%63/%7bfoo%7d"));


  REQUIRE(uri1 != uri2);
  uri2.NormalizePath();
  REQUIRE(uri1 == uri2);
}

TEST_CASE("Resolve relative refence form a base Uri", "Uri")
{
  struct TestVector
  {
    std::string base;
    std::string relative_reference_string;
    std::string target_string;
  };

  const std::vector<TestVector> testVectors{
    { "http://a/b/c/d;p?q", "g:h", "g:h" },
    { "http://a/b/c/d;p?q", "g", "http://a/b/c/g" },
    { "http://a/b/c/d;p?q", "./g", "http://a/b/c/g" },
    { "http://a/b/c/d;p?q", "g/", "http://a/b/c/g/" },
    { "http://a/b/c/d;p?q", "//g", "http://g" },
    { "http://a/b/c/d;p?q", "?y", "http://a/b/c/d;p?y" },
    { "http://a/b/c/d;p?q", "g?y", "http://a/b/c/g?y" },
    { "http://a/b/c/d;p?q", "#s", "http://a/b/c/d;p?q#s" },
    { "http://a/b/c/d;p?q", "g#s", "http://a/b/c/g#s" },
    { "http://a/b/c/d;p?q", "g?y#s", "http://a/b/c/g?y#s" },
    { "http://a/b/c/d;p?q", ";x", "http://a/b/c/;x" },
    { "http://a/b/c/d;p?q", "g;x", "http://a/b/c/g;x" },
    { "http://a/b/c/d;p?q", "g;x?y#s", "http://a/b/c/g;x?y#s" },
    { "http://a/b/c/d;p?q", "", "http://a/b/c/d;p?q" },
    { "http://a/b/c/d;p?q", ".", "http://a/b/c/" },
    { "http://a/b/c/d;p?q", "./", "http://a/b/c/" },
    { "http://a/b/c/d;p?q", "..", "http://a/b/" },
    { "http://a/b/c/d;p?q", "../", "http://a/b/" },
    { "http://a/b/c/d;p?q", "../g", "http://a/b/g" },
    { "http://a/b/c/d;p?q", "../..", "http://a/" },
    { "http://a/b/c/d;p?q", "../../", "http://a/" },
    { "http://a/b/c/d;p?q", "../../g", "http://a/g" },
    { "http://example.com", "foo", "http://example.com/foo" },
  };

  for (const auto &testVector : testVectors) {
    Uri::Uri base_uri;
    Uri::Uri relative_uri;
    Uri::Uri expected_uri;

    INFO("Relative uri: " + testVector.relative_reference_string);
    REQUIRE(base_uri.ParseFromString(testVector.base));
    REQUIRE(relative_uri.ParseFromString(testVector.relative_reference_string));
    REQUIRE(expected_uri.ParseFromString(testVector.target_string));

    const auto actual_target_uri = base_uri.Resolve(relative_uri);

    REQUIRE(expected_uri == actual_target_uri);
  }
}

TEST_CASE("Empty path in Uri whit authority is equivalent to slash only path",
  "Uri")
{
  Uri::Uri uri1;
  Uri::Uri uri2;

  REQUIRE(uri1.ParseFromString("https://example.com"));
  REQUIRE(uri2.ParseFromString("https://example.com/"));
  REQUIRE(uri1 == uri2);

  REQUIRE(uri1.ParseFromString("urn:"));
  REQUIRE(uri2.ParseFromString("urn:/"));
  REQUIRE(uri1 == uri2);

  REQUIRE(uri1.ParseFromString("//example.com"));
  REQUIRE(uri2.ParseFromString("//example.com/"));
  REQUIRE(uri1 == uri2);
}
