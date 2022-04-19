#include "src/cache_manager.hpp"

#include "src/generate_macros.hpp"
#include "src/fs.hpp"
#include "src/output.hpp"

#include <climits>

static std::string get_cache_home_dir() {
   char* cacheHomeDir = std::getenv("XDG_CACHE_HOME");
   if(cacheHomeDir) {
      return cacheHomeDir;
   } else {
      char* homeDir = std::getenv("HOME");
      char buf[PATH_MAX];
      sprintf(buf, "%s/.cache/%s", homeDir, PROJECT_NAME);
      return buf;
   }
}

namespace app {
   CacheManager::CacheManager(const fs::path& dir) {
      puts(dir.c_str());
   }

   utils::Expected<void> CacheManager::Initialize() {
      fs::path cacheDir = get_cache_home_dir();
      if(fs::exists(cacheDir)) {
         instance.reset(new CacheManager(cacheDir/cache_mgr_dirname));
      } else {
         std::error_code e;
         fs::create_directories(cacheDir/cache_mgr_dirname, e);
         if(e)
            return std::runtime_error("Cache directory: " + e.message());
      }
      
      return {};
   }

   CacheManager* CacheManager::GetInstance() {return instance.get();}
} // namespace app