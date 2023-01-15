#include "../headers/uri.hpp"
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
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

/*
 * This function checks if a character is a lowercase alphabet letter
 *
 * @param [in] character
 * The character to check
 *
 * @return
 * An indication if the character is a lowercase letter
 */

bool IsLowerCase(char letter) { return letter >= 'a' && letter <= 'z'; }

/*
 * This function checks if a character is an uppercase alphabet letter
 *
 * @param [in] letter
 * The character to check
 *
 * @return
 * An indication if the character is an uppercase letter
 */

bool IsUpperCase(char letter) { return letter >= 'A' && letter <= 'Z'; }

/*
 * This function checks if a character is an alphabet letter
 *
 * @param [in] letter
 * The character to check
 *
 * @return
 * An indication if the character is an alphabet letter
 */

bool IsAlphabet(char letter)
{
  return IsLowerCase(letter) || IsUpperCase(letter);
}

/*
 * This function checks if a character is a number
 *
 * @param [in] letter
 * The character to check
 *
 * @return
 * An indication if the character is an number
 */

bool IsNumber(char letter) { return letter >= '0' && letter <= '9'; }

std::function<bool(char, bool)> LegalSchemeCheckStrategy()
{
  auto is_first_character = std::make_shared<bool>(true);

  return [is_first_character](char character, bool end) {
    if (end) {
      return !*is_first_character;
    } else {
      if (*is_first_character) {
        *is_first_character = false;
        return IsAlphabet(character);
      } else {
        return IsAlphabet(character) || character == '+' || character == '-'
               || character == '.' || IsNumber(character);
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

    if (scheme_end == std::string::npos) {
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
      user_name = authority.substr(0, user_delimiter);
      authority = authority.substr(user_delimiter);
    }

    const auto port_delimiter = authority.find(':');
    has_port = false;
    if (port_delimiter == std::string::npos) {
      host = authority;
    } else {
      host = authority.substr(0, port_delimiter);
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

  void ParsePath(std::string &URL)
  {
    // Parse Path
    // "" -> []
    // "/" -> [""]
    // "foo/" -> [foo, ""]
    // "/foo" -> ["", foo]
    path.clear();
    if (URL == "/") {
      path.emplace_back("");
    } else if (!URL.empty()) {
      for (;;) {
        auto path_delimiter = URL.find('/');
        if (path_delimiter == std::string::npos) {
          path.push_back(URL);
          break;
        } else {
          path.emplace_back(
            URL.begin(), URL.begin() + static_cast<int>(path_delimiter));
          URL = URL.substr(path_delimiter + 1);
        }
      }
    }
  }

  void ParseQueryAndFragment(const std::string &uri_string)
  {
    const auto query_delimiter = uri_string.find('?');

    const auto fragment_delimiter = uri_string.find('#');

    if (fragment_delimiter == std::string::npos) {
      fragment.clear();
      if (query_delimiter == std::string::npos) {
        query.clear();
      } else {
        query = uri_string.substr(query_delimiter + 1);
      }
    } else {
      fragment = uri_string.substr(fragment_delimiter + 1);
      if (query_delimiter == std::string::npos) {
        query.clear();
      } else {
        query = uri_string.substr(
          query_delimiter + 1, fragment_delimiter - query_delimiter - 1);
      }
    }
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

  impl_->ParsePath(uri_left);

  if (!impl_->path.empty()) {
    impl_->ParseQueryAndFragment(impl_->path.back());
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
