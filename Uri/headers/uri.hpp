#ifndef URI_HPP
#define URI_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace Uri {

class Uri
{
public:
  ~Uri();
  Uri(const Uri &) = delete;
  Uri(Uri &&) = delete;
  Uri &operator=(const Uri &) = delete;
  Uri &operator=(Uri &&) = delete;

  Uri();

  bool ParseFromString(const std::string &uri_string);
  [[nodiscard]] std::string GetScheme() const;
  [[nodiscard]] std::string GetHost() const;
  [[nodiscard]] std::vector<std::string> GetPath() const;

  [[nodiscard]] bool HasPort() const;
  [[nodiscard]] uint16_t GetPort() const;

private:
  struct Implementation;

  std::unique_ptr<Implementation> impl_;
};
}// namespace Uri

#endif
