#pragma once
#include "src/fwd.hpp"
#include "src/fs.hpp"

#include <memory>
#include <vector>

namespace app {
   /*
   To save package:
      MAGIC_NUMBER
      DIRECTORY
      RELATIVES_DIRECTORIES
   */
   // Located in $HOME/.cache/$PROGRAM
   class PackageManager {
   public:
      typedef std::vector<Package> PackageList;

      bool savePackage(Package&& pkg);

      const Package* findPackage(const std::string& name) const;
      Package* findPackage(const std::string& name);
      bool hasPackage(const std::string& name) const;
      const PackageList& pkgs() const;

      static PackageManager* GetInstance();
   private:
      static constexpr const char* MAGIC_NUMBER = "lCache";
      static inline std::shared_ptr<PackageManager> instance = nullptr;
      static inline constexpr const char* cache_mgr_dirname = "packages";
      static Package loadPackage(const std::string& pkgName, utils::File& file);
      static fs::path CreateAndGetCacheDirectory();

      PackageManager(const fs::path& dir);
      
      PackageList m_pkgs;
      fs::path m_dir;
   };

std::ostream& operator<<(std::ostream& out, const PackageManager& pkgManager);
} // namespace app