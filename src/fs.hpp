#pragma once
#include <string>

namespace utils {
   std::string get_canonical_path(const std::string& path);
   bool is_directory(const std::string& path);
} // namespace utils