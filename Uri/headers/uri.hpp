#ifndef URI_HPP
#define URI_HPP

#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <vector>

namespace Uri {

class Uri
{
public:
  ~Uri();
  Uri(const Uri &) = delete;
  Uri(Uri &&) noexcept;
  Uri &operator=(const Uri &) = delete;
  Uri &operator=(Uri &&) noexcept;

  Uri();

  bool operator==(const Uri &other) const;
  bool operator!=(const Uri &other) const;

  friend std::ostream &operator<<(std::ostream &out_stream, const Uri &uri);
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
   * This method returns an indication of whether or not the URI contains a
   * absolute path
   *
   * @return
   * An indication of whethe or not URI contains a absolute path
   * */
  [[nodiscard]] bool IsAbsolutePath() const;

  /*
   * This method applies the "remove_dot_segments" routine talked about
   * in  RFC 3986 (https://tools.ietf.org/hotml/rfc3986) to the path
   * segments of the URI in order to normalize the path
   * (apply and remove . and .. segments)
   *
   */
  void NormalizePath();

  /*
   * This method resolves the given relative reference based on the
   * base URI returning the resolverd target URI
   *
   * @param[in] relative_reference
   *    This describes how to get to the target starting at the base
   *
   * @return
   *    The resolved target URI
   */
  [[nodiscard]] Uri Resolve(const Uri &relative_reference) const;

  /**
   * This method sets the scheme of Uri to the given string
   *
   * @param[in] scheme
   *    This is the string which the scheme is set to
   *
   */
  void SetScheme(const std::string &scheme);

  /**
   * This method sets the user name of Uri to the given string
   *
   * @param[in] user_name
   *    This is the string which the user_name is set to
   *
   */
  void SetUserName(const std::string &user_name);

  /**
   * This method sets the host of Uri to the given string
   *
   * @param[in] host
   *    This is the string which the host is set to
   *
   */
  void SetHost(const std::string &host);

  /**
   * This method sets the port of Uri to the given number
   *
   * @param[in] port
   *    This is the number which the port is set to
   *
   */
  void SetPort(const u_int16_t &port);

  /**
   * This method clearthe port of Uri to the given string
   */
  void ClearPort();

  /**
   * This method sets the path of Uri to the given sequence of
   * segments
   *
   * @param[in] path
   *    This is the sequence of segements which the path is set to
   *
   *    An empty string segement can be used at the fron to indicate an
   *    absolute path
   *
   *    An empty string segment can be used at the back to make suer the
   *    path ends in a delismiter when printed put or when combined with
   *    another URI via the Resolve method
   *
   */
  void SetPath(const std::vector<std::string> &path);

  /**
   * This method sets the query of Uri to the given string
   *
   * @param[in] query
   *    This is the string which the query is set to
   *
   */
  void SetQuery(const std::string &query);

  /**
   * This method sets the fragment of Uri to the given string
   *
   * @param[in] fragment
   *    This is the string which the fragment is set to
   *
   */
  void SetFragment(const std::string &fragment);

  /**
   * This method returns a string of the Uri address
   *
   * @param[out]
   *    This is the string of the Uri
   *
   */
  [[nodiscard]] std::string GenerateString() const;

private:
  struct Implementation;

  std::unique_ptr<Implementation> impl_;

  void CopyScheme(const Uri &other);
  void CopyAuthority(const Uri &other);
  void CopyAndNormalizePath(const Uri &other);
  void CopyQuery(const Uri &other);
  void CopyFragment(const Uri &other);
};

}// namespace Uri

#endif
