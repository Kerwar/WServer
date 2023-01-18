#include "../headers/is_character_in_set.hpp"

namespace Uri {
bool IsLowerCase(char letter) { return letter >= 'a' && letter <= 'z'; }
bool IsUpperCase(char letter) { return letter >= 'A' && letter <= 'Z'; }
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

/*
 * This function checks if a character is a hexdigit, the hexidigit values
 * goes from 0-9 and A, B, C, D, E, F
 *
 * @param [in] letter
 * The character to check
 *
 * @return
 * An indication if the character is an hexdigit
 */

bool IsHexDigit(char letter)
{
  return IsNumber(letter) || (letter >= 'A' && letter <= 'F');
}

/*
 * This function checks if a character is an subdelimiter character, this set of
 * character are
 *
 * @param [in] character
 * The character to check
 *
 * @return
 * An indication if the character is an unreserved character
 */

bool IsSubDelimiter(char character)
{
  return character == '!' || character == '$' || character == '&'
         || character == '\'' || character == '(' || character == ')'
         || character == '*' || character == '+' || character == ','
         || character == ';' || character == '=';
}

/*
 * This function checks if a character is an unreserved character, this set of
 * character are ALPHA(lowercase and uppercase letters), [0-9], '+', '-', '.'
 *
 * @param [in] character
 * The character to check
 *
 * @return
 * An indication if the character is an unreserved character
 */

bool IsUnreservedCharacter(char character)
{
  return IsAlphabet(character) || IsNumber(character) || character == '+'
         || character == '-' || character == '.' || character == '~';
}


/*
 * This function checks if a character is an gen-delimiter character, this set
 * of character are ':', '/', '?, '#', '[', ']', '@'
 *
 * @param [in] character
 * The character to check
 *
 * @return
 * An indication if the character is a gen-delimiter character
 */

[[maybe_unused]] bool IsGenDelimiterCharacter(char character)
{
  return character == ':' || character == '/' || character == '?'
         || character == '#' || character == '[' || character == ']'
         || character == '@';
}

/*
 * This function checks if a character is an pchar character, this set
 * of character are unreserved, HEX digit, sud-delimiters, ':' and '@'
 *
 * @param [in] character
 * The character to check
 *
 * @return
 * An indication if the character is a pchar character
 */

[[maybe_unused]] bool IsPcharCharacter(char character)
{
  return IsUnreservedCharacter(character) || IsSubDelimiter(character)
         || character == ':' || character == '@';
}

}// namespace Uri
