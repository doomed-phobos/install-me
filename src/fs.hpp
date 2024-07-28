#pragma once
#include "generate_macros.hpp"

#include <filesystem>

namespace fs = std::filesystem;

namespace utils {
  inline fs::path get_canonical(const fs::path& path) {
    std::error_code e;
    auto canonical_path = fs::canonical(path, e);
    if(e)
      throw std::runtime_error("'" + path.string() + "': " + e.message());

    return canonical_path;
  }

  inline std::string get_cache_home_dir() {
#ifdef APP_UNIX
    char* cacheHomeDir = std::getenv("XDG_CACHE_HOME");
    if(cacheHomeDir) {
      return cacheHomeDir;
    } else {
      return std::format("{}/.cache/{}", std::getenv("HOME"), PROJECT_NAME); 
    }
#elifdef APP_WINDOWS
  #error "Implement this"
#else
  #error "Implement this"
#endif
  }
} // namespace utils