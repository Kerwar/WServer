#include "../headers/uri.hpp"
#include <cstdint>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>

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
};

Uri::~Uri() = default;

Uri::Uri() : impl_(new Implementation) {}

bool Uri::ParseFromString(const std::string &uri_string)
{
  auto uri_after_scheme = ParseScheme(uri_string);

  impl_->has_port = false;

  auto authority_end = uri_after_scheme.find('/');

  if (authority_end == std::string::npos) {
    auto other_authority_end = uri_after_scheme.find_first_of("?#");
    authority_end = other_authority_end == std::string::npos
                      ? uri_after_scheme.length()
                      : other_authority_end;
  }

  auto authority = uri_after_scheme.substr(0, authority_end);
  auto path = uri_after_scheme.substr(authority_end);

  auto user_delimiter = authority.find('@');
  if (user_delimiter == std::string::npos) {
    impl_->user_name.clear();
  } else {
    impl_->user_name = authority.substr(0, user_delimiter);
    authority = authority.substr(user_delimiter);
  }

  const auto port_delimiter = authority.find(':');
  if (port_delimiter == std::string::npos) {
    impl_->host = authority;
  } else {
    impl_->host = authority.substr(0, port_delimiter);
    try {
      const std::shared_ptr<size_t> end_of_number(new size_t);
      const std::string port_segment = authority.substr(
        port_delimiter + 1, authority_end - port_delimiter - 1);

      impl_->port =
        static_cast<uint16_t>(std::stoul(port_segment, end_of_number.get()));
      const auto signed_port = std::stoi(port_segment);

      if (*end_of_number != port_segment.size()) { return false; }
      if (signed_port != impl_->port) { return false; }

    } catch (const std::invalid_argument &e) {
      return false;
    }

    impl_->has_port = true;
    authority = authority.substr(authority_end);
  }

  ParsePath(path);

  if (!impl_->path.empty()) { ParseQueryAndFragment(impl_->path.back()); }

  return true;
}

std::string Uri::ParseScheme(const std::string &uri_string)
{
  auto scheme_end = uri_string.find(':');

  if (scheme_end == std::string::npos) {
    impl_->scheme.clear();
    return uri_string;
  } else {
    impl_->scheme = uri_string.substr(0, scheme_end);
    return uri_string.substr(scheme_end + 3);
  }
}

std::string Uri::ParseHost(const std::string &uri_string)
{
  auto uri_after_scheme = uri_string;
  impl_->host = uri_after_scheme;
  return uri_after_scheme;
}

void Uri::ParsePath(std::string &URL)
{
  // Parse Path
  // "" -> []
  // "/" -> [""]
  // "foo/" -> [foo, ""]
  // "/foo" -> ["", foo]
  impl_->path.clear();
  if (URL == "/") {
    impl_->path.emplace_back("");
  } else if (!URL.empty()) {
    for (;;) {
      auto path_delimiter = URL.find('/');
      if (path_delimiter == std::string::npos) {
        impl_->path.push_back(URL);
        break;
      } else {
        impl_->path.emplace_back(
          URL.begin(), URL.begin() + static_cast<int>(path_delimiter));
        URL = URL.substr(path_delimiter + 1);
      }
    }
  }
}

void Uri::ParseQueryAndFragment(const std::string &uri_string)
{
  const auto query_delimiter = uri_string.find('?');

  const auto fragment_delimiter = uri_string.find('#');

  if (fragment_delimiter == std::string::npos) {
    impl_->fragment.clear();
    if (query_delimiter == std::string::npos) {
      impl_->query.clear();
    } else {
      impl_->query = uri_string.substr(query_delimiter + 1);
    }
  } else {
    impl_->fragment = uri_string.substr(fragment_delimiter + 1);
    if (query_delimiter == std::string::npos) {
      impl_->query.clear();
    } else {
      impl_->query = uri_string.substr(
        query_delimiter + 1, fragment_delimiter - query_delimiter - 1);
    }
  }
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
