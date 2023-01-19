#include "../headers/uri.hpp"
#include "character_in_set.hpp"
#include "percent_encoded_character_decoder.hpp"

#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>

namespace {
/*
 * This Function checks if every character of the string matches the pattern
 * the requisites from another Function
 *
 * @param [in] candidate
 *  This is the string to test
 *
 *@param[in] StillPassing
 * This is the requisite Function
 *
 * @return
 * An indication if the candidate passes the test
 */

bool FailsMatch(const std::string &candidate,
  const std::function<bool(char, bool)> &StillPassing)
{
  for (const auto character : candidate) {
    if (!StillPassing(character, false)) { return true; }
  }
  return !StillPassing(' ', true);
}

std::function<bool(char, bool)> LegalSchemeCheckStrategy()
{
  auto is_first_character = std::make_shared<bool>(true);

  return [is_first_character](char character, bool end) {
    if (end) {
      return !*is_first_character;
    } else {
      if (*is_first_character) {
        *is_first_character = false;
        return Uri::IsCharacterInSet(character, Uri::ALPHA);
      } else {
        return Uri::IsCharacterInSet(character, Uri::SCHEME_NOT_FIRST);
      }
    }
  };
}


}// namespace


namespace Uri {

struct Uri::Implementation
{

  std::string scheme;
  std::string user_name;
  std::string host;
  uint16_t port = 0000;
  bool has_port = false;
  std::vector<std::string> path;
  std::string query;
  std::string fragment;

  // Methods
  bool ParseScheme(const std::string &uri_string)
  {
    auto scheme_end = uri_string.find(':');

    if (scheme_end == std::string::npos || scheme_end > uri_string.find('/')) {
      scheme.clear();
      return true;
    } else {
      scheme = uri_string.substr(0, scheme_end);
      return !FailsMatch(scheme, LegalSchemeCheckStrategy());
    }
  }

  bool ParseHost(std::string &uri_string)
  {
    auto authority_end = uri_string.find('/', 2);

    if (authority_end == std::string::npos) {
      auto other_authority_end = uri_string.find_first_of("?#");
      authority_end = other_authority_end == std::string::npos
                        ? uri_string.length()
                        : other_authority_end;
    }

    auto authority = uri_string.substr(2, authority_end - 2);
    uri_string = uri_string.substr(authority_end);

    auto user_delimiter = authority.find('@');
    if (user_delimiter == std::string::npos) {
      user_name.clear();
    } else {
      auto coded_user_name = authority.substr(0, user_delimiter);
      authority = authority.substr(user_delimiter + 1);

      if (!UncodeUserName(coded_user_name)) { return false; }
    }

    auto port_delimiter = authority.find(':');
    if (port_delimiter < authority.find(']')
        && authority.find(']') != std::string::npos) {
      port_delimiter = authority.find(':', port_delimiter + 1);
    }

    has_port = false;

    if (port_delimiter == std::string::npos) {
      if (!UncodeHost(authority)) { return false; }
    } else {
      auto coded_host = authority.substr(0, port_delimiter);
      if (!UncodeHost(coded_host)) { return false; }
      try {
        const std::shared_ptr<size_t> end_of_number(new size_t);
        const std::string port_segment = authority.substr(
          port_delimiter + 1, authority_end - port_delimiter - 1);

        port =
          static_cast<uint16_t>(std::stoul(port_segment, end_of_number.get()));
        const auto signed_port = std::stoi(port_segment);

        if (*end_of_number != port_segment.size()) { return false; }
        if (signed_port != port) { return false; }

      } catch (const std::invalid_argument &e) {
        return false;
      }

      has_port = true;
    }
    return true;
  }

  bool UncodeUserName(const std::string &coded_user_name)
  {
    enum Decode_state { normal_state, hex_decode_character };

    Decode_state decode_state = normal_state;
    PercentEncodedCharacterDecoder percent_decoder;

    for (auto character : coded_user_name) {
      switch (decode_state) {
      case normal_state:
        if (character == '%') {
          percent_decoder = PercentEncodedCharacterDecoder();
          decode_state = hex_decode_character;
          break;
        } else if (IsCharacterInSet(character, USER_NAME)) {
          user_name.push_back(character);
          break;
        }
        return false;

      case hex_decode_character:
        if (!percent_decoder.NextEncodedCharacter(character)) { return false; }
        if (percent_decoder.Done()) {
          user_name.push_back(percent_decoder.GetDecodedCharacter());
          decode_state = normal_state;
        }
      }
    }
    return decode_state == normal_state;
  }

  bool UncodeHost(const std::string &coded_host)
  {
    enum Decoded_state {
      first_character,
      normal_state,
      hex_decode_character,
      IPLiteral,
      IPv4address,
    };

    Decoded_state decode_state =
      coded_host.empty() ? normal_state : first_character;
    PercentEncodedCharacterDecoder percent_decoder;

    for (auto character : coded_host) {

      switch (decode_state) {
      case first_character:
        if (character == '[') {
          decode_state = IPLiteral;
          break;
        }
        decode_state = normal_state;
        [[fallthrough]];

      case normal_state:
        if (character == '%') {
          percent_decoder = PercentEncodedCharacterDecoder();
          decode_state = hex_decode_character;
          break;
        } else if (IsCharacterInSet(character, REG_NAME_NOT_PCT_ENCODED)) {
          host.push_back(character);
          break;
        }
        return false;

      case hex_decode_character:
        if (!percent_decoder.NextEncodedCharacter(character)) { return false; }
        if (percent_decoder.Done()) {
          host.push_back(percent_decoder.GetDecodedCharacter());
          decode_state = normal_state;
        }
        break;

      case IPLiteral:
        return DecodeIP(coded_host);

      case IPv4address:
        break;
      }
    }

    return decode_state == normal_state;
  }

  bool DecodeIP(const std::string &coded_host)
  {
    enum States {
      first_character,
      IP_literal,
      IPv6address,
      IPvFuture_hexdigit,
      IPvFuture_dot,
      IPvFuture_last,
      last_character,
    };

    States decode_state = first_character;
    for (auto character : coded_host) {
      switch (decode_state) {
      case first_character:
        host.push_back(character);
        decode_state = IP_literal;
        break;

      case IP_literal:
        if (character == 'v') {
          decode_state = IPvFuture_hexdigit;
          host.push_back(character);
          break;
        } else {
          decode_state = IPv6address;
        }
        [[fallthrough]];

      case IPv6address:
        if (character == ']') {
          host.push_back(character);
          break;
        }
        return false;

      case IPvFuture_hexdigit:
        if (IsCharacterInSet(character, HEX_DIGIT)) {
          host.push_back(character);
          decode_state = IPvFuture_dot;
          break;
        }
        return false;

      case IPvFuture_dot:
        if (character == '.') {
          host.push_back(character);
          decode_state = IPvFuture_last;
          break;
        }
        return false;

      case IPvFuture_last:
        if (IsCharacterInSet(character, IPVFUTURE_LAST)) {
          host.push_back(character);
          decode_state = last_character;
          break;
        }
        return false;

      case last_character:
        if (character == ']' && host.back() != ']') {
          host.push_back(character);
          break;
        }
        [[fallthrough]];

      default:
        return false;
      }
    }

    return true;
  }

  bool ParsePath(std::string &URL)
  {
    // Parse Path
    // "" -> []
    // "/" -> [""]
    // "foo/" -> [foo, ""]
    // "/foo" -> ["", foo]
    path.clear();
    if (URL == "/") {
      path.emplace_back("");
      return true;
    }

    if (!URL.empty()) {
      for (;;) {
        auto path_delimiter = URL.find('/');
        auto query_fragment_delimiter = std::min(URL.find('?'), URL.find('#'));

        if (path_delimiter == std::string::npos) {
          path_delimiter = query_fragment_delimiter;
          if (path_delimiter == std::string::npos) {
            path.push_back(URL);
            URL.clear();
          } else {
            path.emplace_back(
              URL.begin(), URL.begin() + static_cast<int>(path_delimiter));
          }
          break;
        } else {
          if (path_delimiter > query_fragment_delimiter) {
            path.emplace_back(URL.begin(),
              URL.begin() + static_cast<int>(query_fragment_delimiter));
            URL = URL.substr(query_fragment_delimiter);
            break;
          }
          path.emplace_back(
            URL.begin(), URL.begin() + static_cast<int>(path_delimiter));
          URL = URL.substr(path_delimiter + 1);
        }
      }
    }

    for (auto &segment : path) {
      if (!DecodeSegmentPath(segment)) { return false; }
    }

    return true;
  }

  bool static DecodeSegmentPath(std::string &segment)
  {
    const auto original_segment = std::move(segment);
    segment.clear();

    enum Decode_state { normal_state, hex_decode_character };

    PercentEncodedCharacterDecoder percent_decoder;
    Decode_state decode_state = normal_state;

    for (auto character : original_segment) {

      switch (decode_state) {
      case normal_state:
        if (character == '%') {
          percent_decoder = PercentEncodedCharacterDecoder();
          decode_state = hex_decode_character;
          break;
        } else if (IsCharacterInSet(character, PCHAR_NOT_PCT_ENCODED)) {
          segment.push_back(character);
          break;
        }
        return false;

      case hex_decode_character:
        if (!percent_decoder.NextEncodedCharacter(character)) { return false; }
        if (percent_decoder.Done()) {
          segment.push_back(percent_decoder.GetDecodedCharacter());
          decode_state = normal_state;
        }
        break;
      }
    }

    return decode_state == normal_state;
  }

  bool ParseQueryAndFragment(const std::string &uri_string)
  {
    const auto query_delimiter = uri_string.find('?');

    const auto fragment_delimiter = uri_string.find('#');

    if (fragment_delimiter == std::string::npos) {
      fragment.clear();
      if (query_delimiter == std::string::npos) {
        query.clear();
      } else {
        query = uri_string.substr(query_delimiter + 1);
        if (!DecodeQueryOrFragment(query)) {
          query.clear();
          return false;
        }
      }
    } else {
      fragment = uri_string.substr(fragment_delimiter + 1);
      if (!DecodeQueryOrFragment(fragment)) {
        fragment.clear();
        return false;
      }
      if (query_delimiter == std::string::npos) {
        query.clear();
      } else {
        query = uri_string.substr(
          query_delimiter + 1, fragment_delimiter - query_delimiter - 1);
        if (!DecodeQueryOrFragment(query)) {
          query.clear();
          return false;
        }
      }
    }
    return true;
  }

  bool static DecodeQueryOrFragment(std::string &uri_string)
  {
    auto original_string = std::move(uri_string);
    uri_string.clear();

    enum Decode_state { normal_state, hex_decode_character };

    Decode_state decode_state = normal_state;
    PercentEncodedCharacterDecoder percent_decoder;

    for (auto character : original_string) {

      switch (decode_state) {
      case normal_state:
        if (character == '%') {
          decode_state = hex_decode_character;
          percent_decoder = PercentEncodedCharacterDecoder();
          break;
        } else if (IsCharacterInSet(character, QUERY_OR_FRAGMENT)) {
          uri_string.push_back(character);
          break;
        }
        return false;

      case hex_decode_character:
        if (!percent_decoder.NextEncodedCharacter(character)) { return false; }
        if (percent_decoder.Done()) {
          uri_string.push_back(percent_decoder.GetDecodedCharacter());
          decode_state = normal_state;
        }
        break;
      }
    }
    return decode_state == normal_state;
  }
};

Uri::~Uri() = default;

Uri::Uri() : impl_(new Implementation) {}

bool Uri::ParseFromString(const std::string &uri_string)
{
  if (!impl_->ParseScheme(uri_string)) { return false; }

  auto scheme_end = uri_string.find(':');
  auto uri_left =
    impl_->scheme.empty() ? uri_string : uri_string.substr(scheme_end + 1);

  if (uri_left.substr(0, 2) == "//") {
    if (!impl_->ParseHost(uri_left)) { return false; }
  }

  if (!impl_->ParsePath(uri_left)) { return false; }

  if (!uri_left.empty()) {
    if (!impl_->ParseQueryAndFragment(uri_left)) { return false; };
  }

  return true;
}

std::string Uri::GetScheme() const { return impl_->scheme; }

std::string Uri::GetUserName() const { return impl_->user_name; }

std::string Uri::GetHost() const { return impl_->host; }

std::vector<std::string> Uri::GetPath() const { return impl_->path; }

bool Uri::HasPort() const { return impl_->has_port; }

uint16_t Uri::GetPort() const { return impl_->port; }

std::string Uri::GetQuery() const { return impl_->query; }

std::string Uri::GetFragment() const { return impl_->fragment; }

bool Uri::IsRelativeReference() const { return impl_->scheme.empty(); }

bool Uri::IsRelativePath() const
{
  if (impl_->path.empty()) {
    return true;
  } else {
    return !impl_->path[0].empty();
  }
}

}// namespace Uri
