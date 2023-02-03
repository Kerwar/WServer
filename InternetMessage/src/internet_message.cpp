#include "internet_message.hpp"
#include <algorithm>

namespace {

/** These are the characters thar are considered whitespace*/
constexpr const char *WHITESPACE = " \r\n\t";
/**
 * This function returns a copy of the given string, whith any whitespace
 * at the begging and end stripped off.
 *
 * @param[in] rawString
 *  This is the string to strip.
 *
 * @return
 *  The stripped string is reutrned.
 */
std::string StripWhiteSpaceMargins(const std::string &rawString)
{
  const auto marginLeft = rawString.find_first_not_of(WHITESPACE);
  const auto marginRight = rawString.find_last_not_of(WHITESPACE);

  if (marginLeft == std::string::npos) {
    return "";
  } else {
    return rawString.substr(marginLeft, marginRight - marginLeft + 1);
  }
}

};// namespace

namespace InternetMessage {

InternetMessage::Header::Header(HeaderName newName, HeaderValue newValue)
  : name(std::move(newName)), value(std::move(newValue))
{}

struct InternetMessage::Implementation
{
  Headers headers;
  std::string body;
};

InternetMessage::~InternetMessage() = default;

InternetMessage::InternetMessage() : impl_(new Implementation) {}

bool InternetMessage::ParseFromString(const std::string &rawMessage)
{
  size_t offset = 0;

  while (offset < rawMessage.size()) {
    auto lineTerminator = rawMessage.find("\r\n", offset);
    if (lineTerminator == std::string::npos) { break; }
    if (lineTerminator == offset) {
      offset = lineTerminator + 2;
      break;
    }
    auto nameValueDelimiter = rawMessage.find(':', offset);
    if (nameValueDelimiter == std::string::npos) { return false; }

    HeaderName name = rawMessage.substr(offset, nameValueDelimiter - offset);
    HeaderValue value =
      rawMessage.substr(nameValueDelimiter + 1, lineTerminator - nameValueDelimiter);
    impl_->headers.emplace_back(name, StripWhiteSpaceMargins(value));
    offset = lineTerminator + 2;
  }

  impl_->body = rawMessage.substr(offset);
  return true;
}

auto InternetMessage::GetHeaders() const -> Headers { return impl_->headers; }

std::string InternetMessage::GetBody() const { return impl_->body; }

bool InternetMessage::HasHeader(const HeaderName &name) const
{
  return std::any_of(impl_->headers.begin(), impl_->headers.end(), [name](const Header &header) {
    return header.name == name;
  });
}


}// namespace InternetMessage
