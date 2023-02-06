#include "normalize_case_insensitive_string.hpp"

#include <algorithm>
#include <cctype>

namespace Uri {

std::string NormalizeCaseInsensitiveString(const std::string &in_string)
{

  std::string out_string;

  std::transform(in_string.begin(), in_string.end(), std::back_inserter(out_string), tolower);

  return out_string;
}

}// namespace Uri
