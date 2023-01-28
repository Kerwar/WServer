#include "uri.hpp"
#include "character_set.hpp"
#include "normalize_case_insensitive_string.hpp"
#include "percent_encoded_character_decoder.hpp"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <iterator>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/types.h>

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
        return Uri::ALPHA.Contains(character);
      } else {
        return Uri::SCHEME_NOT_FIRST.Contains(character);
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

  [[nodiscard]] bool HasAuthority() const
  {
    return !host.empty() || !user_name.empty() || has_port;
  }

  bool ParseScheme(const std::string &uri_string)
  {
    auto scheme_end = uri_string.find(':');

    if (scheme_end == std::string::npos || scheme_end > uri_string.find('/')) {
      scheme.clear();
      return true;
    } else {
      scheme = uri_string.substr(0, scheme_end);
      scheme = NormalizeCaseInsensitiveString(scheme);
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
      user_name = authority.substr(0, user_delimiter);
      authority = authority.substr(user_delimiter + 1);

      if (!DecodeElement(user_name, USER_NAME)) { return false; }
    }

    auto port_delimiter =
      authority[0] != '[' ? authority.find(':') : std::string::npos;
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

  bool UncodeHost(const std::string &coded_host)
  {
    enum class Decoded_state {
      first_character,
      normal_state,
      hex_decode_character,
      IPLiteral,
      IPv4address,
    };

    Decoded_state decode_state = coded_host.empty()
                                   ? Decoded_state::normal_state
                                   : Decoded_state::first_character;
    PercentEncodedCharacterDecoder percent_decoder;

    for (auto character : coded_host) {

      switch (decode_state) {
      case Decoded_state::first_character:
        if (character == '[') {
          decode_state = Decoded_state::IPLiteral;
          break;
        }
        decode_state = Decoded_state::normal_state;
        [[fallthrough]];

      case Decoded_state::normal_state:
        if (character == '%') {
          percent_decoder = PercentEncodedCharacterDecoder();
          decode_state = Decoded_state::hex_decode_character;
          break;
        } else if (REG_NAME_NOT_PCT_ENCODED.Contains(character)) {
          host.push_back(character);
          break;
        }
        return false;

      case Decoded_state::hex_decode_character:
        if (!percent_decoder.NextEncodedCharacter(character)) { return false; }
        if (percent_decoder.Done()) {
          host.push_back(percent_decoder.GetDecodedCharacter());
          decode_state = Decoded_state::normal_state;
        }
        break;

      case Decoded_state::IPLiteral:
        return DecodeIP(coded_host);

      case Decoded_state::IPv4address:
        break;
      }
    }

    host = NormalizeCaseInsensitiveString(host);
    return decode_state == Decoded_state::normal_state;
  }

  bool DecodeIP(const std::string &coded_host)
  {
    const std::string inside_brackets =
      coded_host.substr(1, coded_host.length() - 2);
    if (coded_host[0] != '[' || coded_host.back() != ']') { return false; }
    if (inside_brackets[0] == 'v') { return DecodeIPvFuture(inside_brackets); }

    return ValidateIpv6Address(inside_brackets);
  }

  bool DecodeIPvFuture(const std::string &coded_host)
  {
    enum class States {
      prefix,
      hexdigit,
      dot,
      sufix,
    };

    States decode_state = States::prefix;

    for (auto character : coded_host) {
      switch (decode_state) {
      case States::prefix:
        if (character != 'v') { return false; }
        host.push_back(character);
        decode_state = States::hexdigit;
        break;

      case States::hexdigit:
        if (!HEX_DIGIT.Contains(character)) { return false; }
        host.push_back(character);
        decode_state = States::dot;
        break;

      case States::dot:
        if (character != '.') { return false; }
        host.push_back(character);
        decode_state = States::sufix;
        break;

      case States::sufix:
        if (!IPVFUTURE_LAST.Contains(character)) { return false; }
        host.push_back(character);
        break;
      }
    }

    return decode_state == States::sufix;
  }

  bool ValidateIpv6Address(const std::string &address)// NOLINT
  {
    size_t number_groups = 0;
    size_t number_digits = 0;
    bool double_colon_encountered = false;
    size_t potential_ipv4_start = 0;
    size_t position = 0;
    bool ipv4_encountered = false;

    enum class ValidationState {
      NoGroupYet,
      ColonNoGroupYet,
      MaybeIPV4,
      NotIPV4,
      AfterDoubleColon,
      ColonAfterGroup,
    };

    ValidationState current_state = ValidationState::NoGroupYet;
    for (const auto &character : address) {
      switch (current_state) {
      case ValidationState::NoGroupYet:
        if (character == ':') {
          current_state = ValidationState::ColonNoGroupYet;
          break;
        } else if (DIGITS.Contains(character)) {
          potential_ipv4_start = position;
          number_digits = 1;
          current_state = ValidationState::MaybeIPV4;
          break;
        } else if (HEX_DIGIT.Contains(character)) {
          current_state = ValidationState::NotIPV4;
          number_digits = 1;
          break;
        }
        return false;

      case ValidationState::ColonNoGroupYet:
        if (character == ':') {
          double_colon_encountered = true;
          current_state = ValidationState::AfterDoubleColon;
          break;
        }
        return false;

      case ValidationState::AfterDoubleColon:
        if (DIGITS.Contains(character)) {
          potential_ipv4_start = position;
          if (++number_digits > 4) { return false; }
          current_state = ValidationState::MaybeIPV4;
          break;
        } else if (HEX_DIGIT.Contains(character)) {
          if (++number_digits > 4) { return false; }
          current_state = ValidationState::NotIPV4;
          break;
        }
        return false;

      case ValidationState::NotIPV4:
        if (character == ':') {
          number_digits = 0;
          number_groups++;
          current_state = ValidationState::ColonAfterGroup;
          break;
        } else if (HEX_DIGIT.Contains(character)) {
          if (++number_digits > 4) { return false; }
          break;
        }
        return false;

      case ValidationState::MaybeIPV4:
        if (character == ':') {
          number_digits = 0;
          number_groups++;
          current_state = ValidationState::ColonAfterGroup;
          break;
        } else if (character == '.') {
          ipv4_encountered = true;
          break;
        } else if (DIGITS.Contains(character)) {
          if (++number_digits > 4) { return false; }
          break;
        } else if (HEX_DIGIT.Contains(character)) {
          if (++number_digits > 4) { return false; }
          current_state = ValidationState::NotIPV4;
          break;
        }
        return false;

      case ValidationState::ColonAfterGroup:
        if (character == ':') {
          if (double_colon_encountered) { return false; }
          double_colon_encountered = true;
          current_state = ValidationState::AfterDoubleColon;
          break;
        } else if (DIGITS.Contains(character)) {
          potential_ipv4_start = position;
          ++number_digits;
          current_state = ValidationState::MaybeIPV4;
          break;
        } else if (HEX_DIGIT.Contains(character)) {
          if (++number_digits > 4) { return false; }
          ++number_digits;
          current_state = ValidationState::NotIPV4;
          break;
        }
        return false;
      }
      if (ipv4_encountered) { break; }
      ++position;
    }

    if (current_state == ValidationState::NotIPV4
        || current_state == ValidationState::MaybeIPV4) {
      ++number_groups;
    }
    if (position == address.length()
        && (current_state == ValidationState::ColonNoGroupYet
            || current_state == ValidationState::ColonAfterGroup)) {
      return false;
    }

    const size_t MAX_GROUPS = 8;

    if (ipv4_encountered) {
      if (!ValidateIpv4Address(address.substr(potential_ipv4_start))) {
        return false;
      }
      number_groups += 2;
    }

    host = address;
    if (double_colon_encountered) {
      return number_groups < MAX_GROUPS;
    } else {
      return number_groups == MAX_GROUPS;
    }
  }

  static bool ValidateIpv4Address(const std::string &address)
  {
    const size_t MAX_GROUPS = 4;
    size_t number_groups = 0;
    std::string octet_buffer;

    enum class ValidationState { OutOctet, DigitOrDot };

    ValidationState current_state = ValidationState::OutOctet;

    for (const auto &character : address) {
      switch (current_state) {
      case ValidationState::OutOctet:
        if (!DIGITS.Contains(character)) { return false; }
        octet_buffer.push_back(character);
        current_state = ValidationState::DigitOrDot;
        break;

      case ValidationState::DigitOrDot:
        if (character == '.') {
          if (++number_groups > 4) { return false; }
          if (!ValidateOctet(octet_buffer)) { return false; }
          octet_buffer.clear();
          current_state = ValidationState::OutOctet;
          break;
        } else if (DIGITS.Contains(character)) {
          octet_buffer.push_back(character);
          break;
        }
        return false;
      }
    }
    if (!octet_buffer.empty()) {
      ++number_groups;
      if (!ValidateOctet(octet_buffer)) { return false; }
    }

    return number_groups == MAX_GROUPS;
  }

  static bool ValidateOctet(const std::string &octet_string)
  {
    int octet = 0;
    const int OCTET_SHIFT = 10;

    for (const auto &character : octet_string) {
      if (DIGITS.Contains(character)) {
        octet *= OCTET_SHIFT;
        octet += character - '0';
      } else {
        return false;
      }
    }

    const int MAX_OCTET = 255;
    return octet <= MAX_OCTET;
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
      URL.clear();
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
          } else if (path_delimiter != 0) {
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
      if (!DecodeElement(segment, PCHAR_NOT_PCT_ENCODED)) { return false; }
    }

    return true;
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
        if (!DecodeElement(query, QUERY_OR_FRAGMENT)) {
          query.clear();
          return false;
        }
      }
    } else {
      fragment = uri_string.substr(fragment_delimiter + 1);
      if (!DecodeElement(fragment, QUERY_OR_FRAGMENT)) {
        fragment.clear();
        return false;
      }
      if (query_delimiter == std::string::npos) {
        query.clear();
      } else {
        query = uri_string.substr(
          query_delimiter + 1, fragment_delimiter - query_delimiter - 1);
        if (!DecodeElement(query, QUERY_OR_FRAGMENT)) {
          query.clear();
          return false;
        }
      }
    }
    return true;
  }

  bool static DecodeElement(std::string &element,
    const CharacterSet &allowed_characters)
  {
    auto coded_string = std::move(element);
    element.clear();

    PercentEncodedCharacterDecoder percent_decoder;
    bool decoding_percent_charcater = false;

    for (auto character : coded_string) {

      if (decoding_percent_charcater) {
        if (!percent_decoder.NextEncodedCharacter(character)) { return false; }
        if (percent_decoder.Done()) {
          decoding_percent_charcater = false;
          element.push_back(percent_decoder.GetDecodedCharacter());
        }
      } else {
        if (character == '%') {
          percent_decoder = PercentEncodedCharacterDecoder();
          decoding_percent_charcater = true;
        } else {
          if (!allowed_characters.Contains(character)) { return false; }
          element.push_back(character);
        }
      }
    }

    return !decoding_percent_charcater;
  }
};

Uri::~Uri() = default;

Uri::Uri() : impl_(new Implementation) {}

bool Uri::operator==(const Uri &other) const
{
  return impl_->scheme == other.impl_->scheme
         && impl_->user_name == other.impl_->user_name
         && impl_->host == other.impl_->host && impl_->port == other.impl_->port
         && impl_->has_port == other.impl_->has_port
         && impl_->path == other.impl_->path
         && impl_->query == other.impl_->query
         && impl_->fragment == other.impl_->fragment;
};

bool Uri::operator!=(const Uri &other) const { return !(*this == other); }

std::ostream &operator<<(std::ostream &out_stream, const Uri &uri)
{
  out_stream << "Scheme: \"" << uri.impl_->scheme << "\"\n";
  out_stream << "User name: \"" << uri.impl_->user_name << "\"\n";
  out_stream << "Host: \"" << uri.impl_->host << "\"\n";
  out_stream << "Port: \"" << uri.impl_->port << "\"\n";
  out_stream << "Path: \"";
  for (const auto &segment : uri.impl_->path) {
    out_stream << segment;
    if (segment != uri.impl_->path.back()) {
      out_stream << "/";
    } else {
      out_stream << "\"\n";
    }
  }
  if (uri.impl_->path.empty()) { out_stream << "\"\n"; }
  out_stream << "Query: \"" << uri.impl_->query << "\"\n";
  out_stream << "Fragment \"" << uri.impl_->fragment << "\"\n";

  return out_stream;
}

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

  if (!impl_->host.empty() && impl_->path.empty()) {
    impl_->path.emplace_back("");
  }

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

bool Uri::IsAbsolutePath() const { return !IsRelativePath(); }

void Uri::NormalizePath()
{
  auto old_path = std::move(impl_->path);

  impl_->path.clear();

  while (!old_path.empty()) {
    if (old_path[0] == ".") {
      if (old_path.size() == 1) { impl_->path.emplace_back(""); }
    } else if (old_path[0] == "..") {
      if (!impl_->path.empty()
          && (!impl_->path[0].empty() || impl_->path.size() > 1)) {
        impl_->path.pop_back();
        if (old_path.size() == 1 && !impl_->path.back().empty()) {
          impl_->path.emplace_back("");
        }
      }
    } else {
      if (!old_path[0].empty() || impl_->path.empty()
          || !impl_->path.back().empty()) {
        impl_->path.push_back(old_path[0]);
      }
    }
    old_path.erase(old_path.begin());
  }
}

Uri Uri::Resolve(const Uri &relative_reference) const
{
  Uri target;

  if (!relative_reference.impl_->scheme.empty()) {
    target.impl_->scheme = relative_reference.impl_->scheme;
    target.CopyAuthority(relative_reference);
    target.CopyAndNormalizePath(relative_reference);
    target.impl_->query = relative_reference.impl_->query;
  } else {
    if (!relative_reference.impl_->host.empty()) {
      target.CopyAuthority(relative_reference);
      target.CopyAndNormalizePath(relative_reference);
      target.impl_->query = relative_reference.impl_->query;
    } else {
      if (relative_reference.impl_->path.empty()) {
        target.CopyAndNormalizePath(*this);
        if (!relative_reference.impl_->query.empty()) {
          target.impl_->query = relative_reference.impl_->query;
        } else {
          target.impl_->query = impl_->query;
        }
      } else if (relative_reference.IsAbsolutePath()) {
        target.impl_->path = relative_reference.impl_->path;
        target.impl_->query = relative_reference.impl_->query;
      } else {
        target.impl_->path = impl_->path;
        if (target.impl_->path.size() > 1) { target.impl_->path.pop_back(); }
        std::copy(relative_reference.impl_->path.begin(),
          relative_reference.impl_->path.end(),
          std::back_inserter(target.impl_->path));
        target.NormalizePath();
        target.impl_->query = relative_reference.impl_->query;
      }
      target.CopyAuthority(*this);
    }
    target.impl_->scheme = impl_->scheme;
  }

  target.impl_->fragment = relative_reference.impl_->fragment;

  return target;
}

void Uri::CopyScheme(const Uri &other) { impl_->scheme = other.impl_->scheme; }

void Uri::CopyAuthority(const Uri &other)
{
  impl_->host = other.impl_->host;
  impl_->user_name = other.impl_->user_name;
  impl_->port = other.impl_->port;
}

void Uri::CopyAndNormalizePath(const Uri &other)
{
  impl_->path = other.impl_->path;
  NormalizePath();
}

void Uri::CopyQuery(const Uri &other) { impl_->query = other.impl_->query; }

void Uri::CopyFragment(const Uri &other)
{
  impl_->fragment = other.impl_->fragment;
}

void Uri::SetScheme(const std::string &scheme) { impl_->scheme = scheme; }

void Uri::SetUserName(const std::string &user_name)
{
  impl_->user_name = user_name;
}

void Uri::SetHost(const std::string &host) { impl_->host = host; }

void Uri::SetPort(const u_int16_t &port)
{
  impl_->port = port;
  impl_->has_port = true;
}

void Uri::ClearPort()
{
  impl_->port = 0;
  impl_->has_port = false;
}


void Uri::SetPath(const std::vector<std::string> &path) { impl_->path = path; }

void Uri::SetQuery(const std::string &query) { impl_->query = query; }

void Uri::SetFragment(const std::string &fragment)
{
  impl_->fragment = fragment;
}

std::string Uri::GenerateString() const
{

  std::ostringstream buffer;

  if (!impl_->scheme.empty()) { buffer << impl_->scheme << ":"; }
  if (impl_->HasAuthority()) {
    buffer << "//";
    if (!impl_->user_name.empty()) { buffer << impl_->user_name << "@"; }
    if (impl_->ValidateIpv6Address(impl_->host)) {
      buffer << '[' << impl_->host << ']';
    } else {
      buffer << impl_->host;
    }
  }

  for (const auto &segment : impl_->path) {
    if (&segment != &(impl_->path.front())
        || (segment.empty() && impl_->path.size() == 1)) {
      buffer << "/";
    }
    buffer << segment;
  }

  if (!impl_->query.empty()) { buffer << "?" << impl_->query; }
  if (!impl_->fragment.empty()) { buffer << "#" << impl_->fragment; }

  return buffer.str();
}

}// namespace Uri
