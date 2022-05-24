#pragma once
#include <string>
#include <cstdarg>

namespace utils {
   std::string fmt_to_str(const char* fmt, ...);
   std::string vfmt_to_str(const char* fmt, std::va_list ap);
   std::string to_lower(const std::string_view& str);
} // namespace utils
