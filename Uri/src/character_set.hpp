#ifndef URI_CHARACTER_IN_SET
#define URI_CHARACTER_IN_SET

#include <initializer_list>
#include <memory>

namespace Uri {

class CharacterSet
{
public:
  ~CharacterSet() noexcept;
  CharacterSet(const CharacterSet &);
  CharacterSet(CharacterSet &&) noexcept;
  CharacterSet &operator=(const CharacterSet &);
  CharacterSet &operator=(CharacterSet &&) noexcept;

  CharacterSet();

  /*
   * This construcs a character set that contains only the character given
   *
   * @param[in] character
   * This is the only character to put int the set
   *
   */
  // cppcheck-suppress noExplicitConstructor
  CharacterSet(char character);

  /*
   * This construcs a character set that contains all the characters
   * betweem the given "first" and "last" characters inclusive
   *
   * @param[in] first
   * This is the first of the range of the characters to put in the set;
   *
   * @param[in] last
   * This is the last of the range of the characters to put in the set;
   */
  CharacterSet(char first, char last);

  /*
   * This construcs a character set with all the sets given
   *
   * @param[in] character_sets
   * This is the list of character sets to merge
   *
   * @param[in] last
   * This is the last of the range of the characters to put in the set;
   */

  // cppcheck-suppress noExplicitConstructor
  CharacterSet(std::initializer_list<const CharacterSet> character_sets);

  /*
   * This construcs a character set with all the sets given
   *
   * @param[in] character_sets
   * This is the list of character sets to merge
   *
   * @param[in] last
   * This is the last of the range of the characters to put in the set;
   */
  [[nodiscard]] bool Contains(char character) const;

private:
  struct Implementation;

  std::unique_ptr<Implementation> impl_;
};

inline const CharacterSet DIGITS{ CharacterSet('0', '9') };
inline const CharacterSet ALPHA{ CharacterSet('A', 'Z'), CharacterSet('a', 'z') };
inline const CharacterSet HEX_DIGIT{ CharacterSet('A', 'F'),
  CharacterSet('a', 'f'),
  CharacterSet('0', '9') };
inline const CharacterSet UNRESERVED{ ALPHA, DIGITS, '-', '.', '_', '~' };
inline const CharacterSet SUB_DELIMS =
  CharacterSet{ '!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '=' };
inline const CharacterSet SCHEME_NOT_FIRST{ ALPHA, DIGITS, '+', '-', '.' };
inline const CharacterSet PCHAR_NOT_PCT_ENCODED{ UNRESERVED, SUB_DELIMS, ':', '@' };
inline const CharacterSet REG_NAME_NOT_PCT_ENCODED{
  UNRESERVED,
  SUB_DELIMS,
  ':',
};
inline const CharacterSet USER_NAME{ UNRESERVED, SUB_DELIMS, ':' };
inline const CharacterSet IPVFUTURE_LAST{ UNRESERVED, SUB_DELIMS, ':', ']' };
inline const CharacterSet QUERY_OR_FRAGMENT{ PCHAR_NOT_PCT_ENCODED, ':', '?', '/' };

}// namespace Uri

#endif// !URI_IS_CHARACTER_IN_SET
