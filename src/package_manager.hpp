#pragma once
#include "fwd.hpp"
#include "fs.hpp"

#include <memory>
#include <vector>

namespace app {
  // Located in $HOME/.cache/$PROGRAM
  class PackageManager {
  public:
    typedef std::vector<std::unique_ptr<Package>> PackageList;
    PackageManager(const PackageManager&) = delete;
    PackageManager& operator=(const PackageManager&) = delete;

    bool installPackage(const PackageInfo& info);
    bool uninstallPackage(const std::string& name);

    const fs::path& cacheDir() const {return m_dir;}
    const PackageList& pkgs() const {return m_pkgs;}

    static PackageManager& GetInstance();
  private:
    PackageManager(const fs::path& dir);
    static fs::path CreateAndGetCacheDirectory();
    static inline constexpr const char* cache_mgr_dirname = "packages";

    Package* createPackage(PackageInfo info);
    Package* loadPackage(const fs::path& filepath);
    Package* findPackage(const std::string& name);
    bool hasPackage(const std::string& name);
    void removePackage(const Package* pkg);
    
    fs::path m_dir;
    /// Lazy initialization
    PackageList m_pkgs;
  };

  std::ostream& operator<<(std::ostream& out, const PackageManager& pkgManager);
} // namespace app