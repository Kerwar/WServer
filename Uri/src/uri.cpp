#include "../headers/uri.hpp"
#include <cstdint>
#include <iterator>
#include <memory>
#include <stdexcept>

namespace Uri {

struct Uri::Implementation
{
  std::string scheme;
  std::string host;
  uint16_t port = 0000;
  bool has_port = false;
  std::vector<std::string> path;
};

Uri::~Uri() = default;

Uri::Uri() : impl_(new Implementation) {}

bool Uri::ParseFromString(const std::string &uri_string)
{
  // Parse scheme
  auto scheme_end = uri_string.find(':');
  impl_->scheme = uri_string.substr(0, scheme_end);
  auto rest = uri_string.substr(scheme_end + 1);

  // Parse Host
  impl_->has_port = false;
  if (rest.substr(0, 2) == "//") {
    const auto authority_end = rest.find('/', 2);
    const auto port_delimiter = rest.find(':');

    if (port_delimiter == std::string::npos || port_delimiter > authority_end) {
      impl_->host = rest.substr(2, authority_end - 2);
      rest = rest.substr(authority_end);
    } else {
      impl_->host = rest.substr(2, port_delimiter - 2);
      try {
        const std::shared_ptr<size_t> end_of_number(new size_t);
        const std::string port_segment = rest.substr(port_delimiter + 1, authority_end - port_delimiter - 1);

        impl_->port = static_cast<uint16_t>(std::stoul(port_segment, end_of_number.get()));
        const auto signed_port = std::stoi(port_segment);

        if (*end_of_number != port_segment.size()) { return false; }
        if (signed_port != impl_->port) { return false; }
      } catch (const std::invalid_argument &e) {
        return false;
      }

      impl_->has_port = true;
      rest = rest.substr(authority_end);
    }
  } else {
    impl_->host.clear();
  }

  // Parse Path
  // "" -> []
  // "/" -> [""]
  // "foo/" -> [foo, ""]
  // "/foo" -> ["", foo]
  impl_->path.clear();
  if (rest == "/") {
    impl_->path.emplace_back("");
  } else if (!rest.empty()) {
    for (;;) {
      auto path_delimiter = rest.find('/');
      if (path_delimiter == std::string::npos) {
        impl_->path.push_back(rest);
        break;
      } else {
        impl_->path.emplace_back(rest.begin(), rest.begin() + static_cast<int>(path_delimiter));
        rest = rest.substr(path_delimiter + 1);
      }
    }
  }
  return true;
}

std::string Uri::GetScheme() const { return impl_->scheme; }

std::string Uri::GetHost() const { return impl_->host; }

std::vector<std::string> Uri::GetPath() const { return impl_->path; }

bool Uri::HasPort() const { return impl_->has_port; }

uint16_t Uri::GetPort() const { return impl_->port; }
}// namespace Uri
