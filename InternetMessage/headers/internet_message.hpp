#ifndef INTERNET_MESSAGE_HPP
#define INTERNET_MESSAGE_HPP

#include <memory>
#include <vector>

namespace InternetMessage {

class InternetMessage
{
public:
  /** This is how we handle the name of a internet message header */
  using HeaderName = std::string;
  /** This is how we handle the value of a internet message header */
  using HeaderValue = std::string;
  struct Header
  {
    /**
     * This is the part of a header that comes before the colon.
     * It identifies the purpose of the header.
     */
    HeaderName name;

    /**
     * This is the part of a header that comes after the colon. It provides the
     * value, setting, or context whose meaning depends on the header name
     */
    HeaderValue value;

    /**
     * This constructor initializes the header's components.
     *
     * @param[in] newName
     * This is the part of a header that comes before the colon.
     * It identifies the purpose of the header.
     *
     * @param[in] newValue
     * This is the part of a header that comes after the colon. It provides the
     * value, setting, or context whose meaning depends on the header name
     */
    Header(HeaderName newName, HeaderValue newValue);
  };

  using Headers = std::vector<Header>;
  /** Default constructor */
  InternetMessage();

  /** Destructor, copy and move operators */
  ~InternetMessage();
  InternetMessage(const InternetMessage &) = delete;
  InternetMessage(InternetMessage &&) = delete;
  InternetMessage operator=(const InternetMessage &) = delete;
  InternetMessage operator=(InternetMessage &&) = delete;

  /**
   * This method determines the headers and body of the message by parsing the
   * raw message from a string
   *
   * @param[in] msgString
   *    This is the string
   *
   * @return
   *    An indication of wheter or not the string has parsed successfully
   */
  bool ParseFromString(const std::string &rawMessage);

  /**
   * This method return the collection of  headers attached to the message
   *
   * @return
   *    The collection of headers attached to the message is returned
   */
  [[nodiscard]] Headers GetHeaders() const;

  /**
   * This method checks to see if there is a header in the message with
   * the given name.
   *
   * @param[in] name
   *    This is the name of the header for which to check.
   *
   * @return
   *    An indication of whether or not there is a header in the message with
   *    the given name is returned
   */
  [[nodiscard]] bool HasHeader(const HeaderName &name) const;

  /**
   * This method returns the part of the message that follows all the headers,
   * and represent the principal content of the overall message
   *
   * @return
   *    The body of the message is returned.
   */
  [[nodiscard]] std::string GetBody() const;

private:
  /**
   * This is the type of structure that contains the private properties of the
   * instance. It is defined in the implmentation and declared here to
   * ensure that it is scoped inside the class.
   */
  struct Implementation;

  /**
   * This constains the private properties of the instance
   */
  std::unique_ptr<Implementation> impl_;
};

}// namespace InternetMessage
#endif// !INTERNET_MESSAGE_HPP
