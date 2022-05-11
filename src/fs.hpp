#pragma once
#include <filesystem>

namespace fs = std::filesystem;

namespace utils {
   static fs::path get_canonical(const fs::path& path) {
         std::error_code e;
         auto canonical_path = fs::canonical(path, e);
         if(e)
            throw std::runtime_error("'" + path.string() + "': " + e.message());

         return canonical_path;
      }
} // namespace utils