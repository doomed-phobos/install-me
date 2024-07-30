#pragma once
#include "generate_macros.hpp"
#include "output.hpp"

#include <filesystem>

namespace fs = std::filesystem;

namespace utils {
  inline fs::path get_canonical(const fs::path& path) {
    std::error_code e;
    auto canonical_path = fs::weakly_canonical(path, e);
    if(e)
      throw std::runtime_error("'" + path.string() + "': " + e.message());

    return canonical_path;
  }

  inline bool create_if_not_exists(const fs::path& path) {
    if(!fs::exists(path)) {
      if(std::error_code e; !fs::create_directory(path, e)) {
        ERROR("Failed to create '{}': {}", path.string(), e.message());
        return false;
      }
    }
    return true;
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