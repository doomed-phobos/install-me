#include "package_manager.hpp"

#include "fs.hpp"
#include "package.hpp"
#include "package_info.hpp"
#include "output.hpp"
#include "package_exception.hpp"
#include "table.hpp"

#include <climits>
#include <cstring>
#include <algorithm>

namespace app {
  PackageManager::PackageManager(const fs::path& dir) :
    m_dir{dir} {
    for(auto entry : fs::directory_iterator(m_dir)) {
        if(!entry.is_directory()) {
          // Check if it's a cache
          auto fPath = entry.path();
          auto pkg = loadPackage(fPath);
          if(!pkg) {
            WARNING("File '{}' isn't a package.", fPath.string());
            continue;
          }
          m_pkgs.emplace_back(pkg);
        }
    }
  }

  Package* PackageManager::loadPackage(const fs::path& filepath) {
    return new UninstallPackage(filepath);
  }

  bool PackageManager::installPackage(const PackageInfo& info) {
    if(hasPackage(info.name)) {
      WARNING("Package '{}' already exists", info.name);
      return false;
    }

    VERBOSE("Installing '{}' package", info.name);
    return (*createPackage(info))(cacheDir());
  }

  Package* PackageManager::findPackage(const std::string& name) {
    auto it = std::ranges::find(m_pkgs, name, [](const auto& e) {return e->info().name;});
    return it != m_pkgs.end() ? it->get() : nullptr;
  }

  bool PackageManager::hasPackage(const std::string& name) {
    return findPackage(name) != nullptr;
  }

  Package* PackageManager::createPackage(PackageInfo info) {
    if(!fs::is_directory(info.inputDir))
      throw std::runtime_error(std::format("Input directory '{}' don't exists", info.inputDir.string()));
    
    INFO("-- PACKAGE INFO '{}' --", info.name);
    INFO("Input dir: {}", info.inputDir.string());
    INFO("Output dir: {}", info.outputDir.string());

    if(auto pkg = findPackage(info.name); pkg)
        throw std::runtime_error(std::format("Package '{}' already exists.", info.name));

    return m_pkgs.emplace_back(new InstallPackage(info)).get();
  }

  bool PackageManager::uninstallPackage(const std::string& name) {
    auto pkg = findPackage(name);
    if(!pkg) {
      ERROR("Uninstall failed: Package '{}' don't exists", name);
      return false;
    }
    
    if((*pkg)(m_dir)) {
      removePackage(pkg);
      VERBOSE("Removing '{}' package from cache", name);
      return true;
    } else
      return false;
  }

  void PackageManager::removePackage(const Package* pkg) {
    m_pkgs.erase(std::ranges::find(m_pkgs, pkg, &PackageList::value_type::get));
  }

  PackageManager& PackageManager::GetInstance() {
    static PackageManager s_instance{CreateAndGetCacheDirectory()};
    
    return s_instance;
  }

  fs::path PackageManager::CreateAndGetCacheDirectory() {
    fs::path cacheDir = utils::get_cache_home_dir();
    if(!fs::exists(cacheDir)) {
        std::error_code e;
        fs::create_directories(cacheDir/cache_mgr_dirname, e);
        if(e)
          throw PackageException("Package directory: " + e.message());
    }

    return cacheDir/cache_mgr_dirname;
  }

  std::ostream& operator<<(std::ostream& out, const PackageManager& pkgManager) {
    utils::table<3> table(1);
    table.setTitle("PACKAGE LISTS");
    table.addRow({"Name", "Path", "Installation directory"});
    for(const auto& pkg : pkgManager.pkgs())
      table.addRow(
        {
          pkg->info().name,
          (pkgManager.cacheDir()/pkg->info().name).string(),
          pkg->info().outputDir.string()
        });

    table.print(out);
    return out;
  }
} // namespace app