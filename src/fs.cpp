#include "src/fs.hpp"

#include <climits>
#include <sys/stat.h>

namespace utils {
   std::string get_canonical_path(const std::string& path) {
      char buf[PATH_MAX];
      realpath(path.c_str(), buf);

      return buf;
   }

   bool is_directory(const std::string& path) {
      struct stat sts;

      return (stat(path.c_str(), &sts) == 0 && S_ISDIR(sts.st_mode)) ? true : false;
   }
} // namespace utils