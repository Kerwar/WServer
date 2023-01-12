#include "../headers/uri.hpp"

namespace Uri {

struct Uri::Implementation
{
  std::string scheme;
  std::string host;
  std::vector<std::string> path;

  std::string path_delimiter = "/";
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
  if (rest.substr(0, 2) == "//") {
    const auto authority_end = rest.find(impl_->path_delimiter, 2);
    impl_->host = rest.substr(2, authority_end - 2);
    rest = rest.substr(authority_end);
  } else {
    impl_->host.clear();
  }

  // Parse Path
  // "" -> []
  // "/" -> [""]
  // "foo/" -> [foo, ""]
  // "/foo" -> ["", foo]
  impl_->path.clear();
  if (rest == impl_->path_delimiter) {
    impl_->path.emplace_back("");
  }
  else if (!rest.empty()) {
    for (;;) {
      auto path_delimiter = rest.find(impl_->path_delimiter);
      if (path_delimiter == std::string::npos) {
        impl_->path.push_back(rest);
        break;
      } else {
        impl_->path.emplace_back(rest.begin(), rest.begin() + static_cast<int>(path_delimiter));
        rest = rest.substr(path_delimiter + impl_->path_delimiter.length());
      }
    }
  }
  return true;
}

std::string Uri::GetScheme() const { return impl_->scheme; }

std::string Uri::GetHost() const { return impl_->host; }

std::vector<std::string> Uri::GetPath() const { return impl_->path; }

bool Uri::SetPathDelimiter(const std::string &path_delimiter)
{
  if (path_delimiter.empty()) {
    return false;
  } else {
    impl_->path_delimiter = path_delimiter;
    return true;
  }
}
}// namespace Uri
