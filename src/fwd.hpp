#pragma once
#include <bits/fs_fwd.h>

namespace fs = std::filesystem;

namespace app {
   class PackageManager;
   class AppFlags;
   struct PackageInfo;
   class AppCliCommands;
   class Package;
} // namespace app

namespace utils {
   class WFile;
   class File;
} // namespace utils