#pragma once
#include <stdexcept>

namespace app {
   class PackageException : public std::runtime_error {
   public:
      PackageException(const std::string& msg) :
         std::runtime_error(msg) {}
   };
} // namespace app