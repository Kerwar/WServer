#ifndef URI_IS_CHARACTER_IN_SET
#define URI_IS_CHARACTER_IN_SET

namespace Uri {

/*
 * This function checks if a character is a lowercase alphabet letter
 *
 * @param [in] character
 * The character to check
 *
 * @return
 * An indication if the character is a lowercase letter
 */

bool IsLowerCase(char letter);

/*
 * This function checks if a character is an uppercase alphabet letter
 *
 * @param [in] letter
 * The character to check
 *
 * @return
 * An indication if the character is an uppercase letter
 */

bool IsUpperCase(char letter);

/*
 * This function checks if a character is an alphabet letter
 *
 * @param [in] letter
 * The character to check
 *
 * @return
 * An indication if the character is an alphabet letter
 */

bool IsAlphabet(char letter);

/*
 * This function checks if a character is a number
 *
 * @param [in] letter
 * The character to check
 *
 * @return
 * An indication if the character is an number
 */

bool IsNumber(char letter);

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

bool IsHexDigit(char letter);

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

bool IsSubDelimiter(char character);

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

bool IsUnreservedCharacter(char character);

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

[[maybe_unused]] bool IsGenDelimiterCharacter(char character);

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

[[maybe_unused]] bool IsPcharCharacter(char character);

}// namespace Uri

#endif// !URI_IS_CHARACTER_IN_SET
