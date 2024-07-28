#pragma once
#include "app_flags.hpp"
#include "fs.hpp"

namespace app {
  struct PackageInfo {
    AppFlags flags;
    fs::path inputDir;
    fs::path outputDir;
    std::string name; // Can be empty
    int nthreads;
  };
} // namespace app