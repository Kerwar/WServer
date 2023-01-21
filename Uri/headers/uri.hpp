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

  /*
   * This method parses the values from a string
   *
   * @input
   * std::string uri_string
   *
   * @output
   * bool if it fails or not
   * */
  bool ParseFromString(const std::string &uri_string);

  /*
   * This method returns the scheme
   *
   * @output
   * std::string the scheme
   *
   * @note
   * if it does not have a string it returns an empty string
   * */
  [[nodiscard]] std::string GetScheme() const;

  /*
   * This method returns the user name
   *
   * std::string the username
   * */
  [[nodiscard]] std::string GetUserName() const;


  /*
   * This method returns the host
   *
   * @output
   * std::string the host
   *
   * @note
   * if it does not have a string it returns an empty string
   * */
  [[nodiscard]] std::string GetHost() const;

  /*
   * This method returns a vector of string for the values of the path
   *
   * @output
   * std::vector<std::string> the path
   *
   * */
  [[nodiscard]] std::vector<std::string> GetPath() const;

  /*
   * This method checks if there is a port
   *
   * @output
   * bool if there is a port
   *
   * @note
   * by defualt is false
   * */
  [[nodiscard]] bool HasPort() const;
  /*
   * This method returns the port
   *
   * @output
   * uint16_t the port
   * */
  [[nodiscard]] uint16_t GetPort() const;

  /*
   * This method returns the query
   *
   * std::string the query
   * */
  [[nodiscard]] std::string GetQuery() const;
  /*
   * This method returns the fragment
   *
   * std::string the fragment
   * */
  [[nodiscard]] std::string GetFragment() const;

  /*
   * This method returns an indication of whether or not the URI is
   * relative reference
   *
   * @return
   * An indication of whethe or not URI is relative reference
   * */
  [[nodiscard]] bool IsRelativeReference() const;

  /*
   * This method returns an indication of whether or not the URI contains a
   * relative path
   *
   * @return
   * An indication of whethe or not URI contains a relative path
   * */
  [[nodiscard]] bool IsRelativePath() const;

  /*
   * This method applies the "remove_dot_segments" routine talked about
   * in  RFC 3986 (https://tools.ietf.org/hotml/rfc3986) to the path
   * segments of the URI in order to normalize the path
   * (apply and remove . and .. segments)
   *
   */
  void NormalizePath();

private:
  struct Implementation;

  std::unique_ptr<Implementation> impl_;
};
}// namespace Uri

#endif
