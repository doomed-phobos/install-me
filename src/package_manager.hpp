#pragma once
#include "src/fwd.hpp"
#include "src/fs.hpp"

#include <memory>
#include <vector>

namespace app {
   // Located in $HOME/.cache/$PROGRAM
   class PackageManager {
   public:
      typedef std::vector<Package*> PackageList;
      ~PackageManager();

      void uninstallPackage(const std::string& name);
      Package& createPackageFromDirectory(const fs::path& input, const fs::path& output, bool symlink = false, const char* nameOrNull = nullptr);

      const Package* findPackage(const std::string& name) const;
      Package* findPackage(const std::string& name);
      bool hasPackage(const std::string& name) const;
      void removePackage(const Package* pkg);

      const PackageList& pkgs() const;
      const fs::path& cacheDir() const;
      static PackageManager* GetInstance();
   private:
      static constexpr const char* MAGIC_NUMBER = "lCache";
      static inline std::shared_ptr<PackageManager> instance = nullptr;
      static inline constexpr const char* cache_mgr_dirname = "packages";
      static fs::path CreateAndGetCacheDirectory();
      
      Package* loadPackage(const fs::path& filepath);
      utils::WFile createCacheFileFromPackage(const Package& pkg);
      void writePathInCacheFile(utils::WFile& cacheFile, const fs::path& path);

      PackageManager(const fs::path& dir);
      
      void copyDirectory(const fs::directory_entry& e, const fs::path& output);
      void copySymLinkDirectory(const fs::directory_entry& e, const fs::path& output);
      
      PackageList m_pkgs;
      fs::path m_dir;
   };

std::ostream& operator<<(std::ostream& out, const PackageManager& pkgManager);
} // namespace app