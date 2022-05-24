#pragma once
#include <stdexcept>

namespace app {
   class ParseException : public std::runtime_error {
   public:
      ParseException(const std::string& msg) :
         std::runtime_error(msg) {}
   };
} // namespace app