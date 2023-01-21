
#ifndef NORMALIZE_CASE_INSENSITEVE_STRING
#define NORMALIZE_CASE_INSENSITEVE_STRING

#include <string>

namespace Uri {
/* This function takes a string and swaps all upper-case characters with their
 * lower-case equivalents, returning the result
 *
 * @param[in} in_string
 *  this is the string to be normalized
 *
 * @param[out]
 *  the normalized string is returned, all upper-case characters have been
 *  replace with their lower-case equivalents
 */

std::string NormalizeCaseInsensitiveString(const std::string &in_string);
}// namespace Uri
 //
#endif
