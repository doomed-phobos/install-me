#include "src/string.hpp"

#include <vector>

namespace utils {
   std::string fmt_to_str(const char* fmt, ...) {
      std::va_list ap;
      va_start(ap, fmt);
      std::string result = vfmt_to_str(fmt, ap);
      va_end(ap);

      return result;
   }

   std::string vfmt_to_str(const char* fmt, std::va_list ap) {
      std::vector<char> buf(1, 0);
      std::va_list ap2;
      va_copy(ap2, ap);
      size_t required_size = std::vsnprintf(nullptr, 0, fmt, ap);
      if(required_size) {
         buf.resize(++required_size);
         std::vsnprintf(&buf[0], buf.size(), fmt, ap2);
      }
      va_end(ap2);

      return std::string(&buf[0]);
   }
} // namespace utils
