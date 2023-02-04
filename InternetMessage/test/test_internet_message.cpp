#include "../headers/internet_message.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Initialization of internet message with https client request message",// NOLINT
  "InternetMessage")
{
  InternetMessage::InternetMessage msg;

  const std::string rawMessage =
    "User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n"
    "Host: www.example.com\r\n"
    "Accept-Language: en, mi\r\n"
    "\r\n";

  REQUIRE(msg.ParseFromRawMessage(rawMessage));

  const auto headers = msg.GetHeaders();

  struct ExpectedHeader
  {
    std::string name;
    std::string value;
  };

  const std::vector<ExpectedHeader> expectedHeaders{
    { "User-Agent", "curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3" },
    { "Host", "www.example.com" },
    { "Accept-Language", "en, mi" },
  };

  REQUIRE(expectedHeaders.size() == headers.size());

  for (size_t i = 0; i < expectedHeaders.size(); ++i) {
    REQUIRE(expectedHeaders[i].name == headers[i].name);
    REQUIRE(expectedHeaders[i].value == headers[i].value);
  }

  REQUIRE(msg.HasHeader("Host"));
  REQUIRE_FALSE(msg.HasHeader("FooBar"));
  INFO(msg.GetBody());
  REQUIRE(msg.GetBody().empty());

  REQUIRE(rawMessage == msg.GenerateRawMessage());
}

TEST_CASE("Initialization of internet message with server response",// NOLINT
  "InternetMessage")
{
  InternetMessage::InternetMessage msg;
  const std::string rawMessage =
    "Date: Mon, 27 Jul 2009 12:28:53 GTM\r\n"
    "Server: Apache\r\n"
    "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
    "ETag: \"34aa387-d-1568eb00\"\r\n"
    "Accept-Ranges: bytes\r\n"
    "Content-Length: 51\r\n"
    "Vary: Accept-Encoding\r\n"
    "Content-Type: text/plain\r\n"
    "\r\n"
    "Hello World! My payload includes a trailing CRLF.\r\n";

  REQUIRE(msg.ParseFromRawMessage(rawMessage));

  const auto headers = msg.GetHeaders();

  struct ExpectedHeader
  {
    std::string name;
    std::string value;
  };

  const std::vector<ExpectedHeader> expectedHeaders{
    { "Date", "Mon, 27 Jul 2009 12:28:53 GTM" },
    { "Server", "Apache" },
    { "Last-Modified", "Wed, 22 Jul 2009 19:15:56 GMT" },
    { "ETag", "\"34aa387-d-1568eb00\"" },
    { "Accept-Ranges", "bytes" },
    { "Content-Length", "51" },
    { "Vary", "Accept-Encoding" },
    { "Content-Type", "text/plain" },
  };

  REQUIRE(expectedHeaders.size() == headers.size());

  for (size_t i = 0; i < expectedHeaders.size(); ++i) {
    REQUIRE(expectedHeaders[i].name == headers[i].name);
    REQUIRE(expectedHeaders[i].value == headers[i].value);
  }

  REQUIRE(msg.HasHeader("Date"));
  REQUIRE(msg.HasHeader("Server"));
  REQUIRE(msg.HasHeader("Last-Modified"));
  REQUIRE(msg.HasHeader("ETag"));
  REQUIRE(msg.HasHeader("Accept-Ranges"));
  REQUIRE(msg.HasHeader("Content-Length"));
  REQUIRE(msg.HasHeader("Vary"));
  REQUIRE(msg.HasHeader("Content-Type"));
  REQUIRE("Hello World! My payload includes a trailing CRLF.\r\n" == msg.GetBody());
  REQUIRE(rawMessage == msg.GenerateRawMessage());
}
