#pragma once
#include "fwd.hpp"
#include "fs.hpp"

#include <memory>
#include <list>

namespace app {
  // Located in $HOME/.cache/$PROGRAM
  class PackageManager {
  public:
    typedef std::list<Package> PackageList;
    PackageManager(const PackageManager&) = delete;
    PackageManager& operator=(const PackageManager&) = delete;

    Package& createPackage(const PackageInfo& info);    

    const fs::path& cacheDir() const {return m_dir;}

    static PackageManager& GetInstance();
  /*public:
    ~PackageManager();

    void uninstallPackage(const std::string& name);
    Package& createPackageFromDirectory(const fs::path& input, const fs::path& output, bool symlink = false, const char* nameOrNull = nullptr);

    const Package* findPackage(const std::string& name) const;
    Package* findPackage(const std::string& name);
    bool hasPackage(const std::string& name) const;
    void removePackage(const Package* pkg);

    const PackageList& pkgs() const;
  private:
    static constexpr const char* MAGIC_NUMBER = "lCache";
    
    Package* loadPackage(const fs::path& filepath);
    utils::WFile createCacheFileFromPackage(const Package& pkg);
    void writePathInCacheFile(utils::WFile& cacheFile, const fs::path& path);

    void copyDirectory(const fs::directory_entry& e, const fs::path& output);
    void copySymLinkDirectory(const fs::directory_entry& e, const fs::path& output);
    
    */
  private:
    PackageManager(const fs::path& dir);
    static fs::path CreateAndGetCacheDirectory();
    static inline constexpr const char* cache_mgr_dirname = "packages";
    
    fs::path m_dir;
    PackageList m_pkgs;
  };

  std::ostream& operator<<(std::ostream& out, const PackageManager& pkgManager);
} // namespace app