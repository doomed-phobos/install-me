#include "package_manager.hpp"

#include "generate_macros.hpp"
#include "fs.hpp"
#include "package.hpp"
#include "package_info.hpp"
#include "file.hpp"
#include "output.hpp"
#include "package_exception.hpp"
#include "table.hpp"

#include <climits>
#include <cstring>
#include <algorithm>

namespace {
  /*  struct same_package {
    same_package(const std::string& name) :
        m_name(name) {}
    
    bool operator()(const app::Package* pkg) const {
        return pkg->name() == m_name;
    }

    std::string m_name;
  };
  */
}

namespace app {
  /*
    PackageManager::PackageManager(const fs::path& dir) :
    m_dir(dir) {
    for(auto entry : fs::directory_iterator(m_dir)) {
        if(!entry.is_directory()) {
          // Check if it's a cache
          auto fPath = entry.path();
          auto pkg = loadPackage(fPath);
          if(!pkg) {
              WARNING(utils::fmt_to_str("File '%s' isn't a package.", fPath.c_str()));
              continue;
          }
          m_pkgs.push_back(pkg);
        }
    }
  }
  */

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
    INFO("Symlink: {}", info.flags.hasFlags(AppFlags::kSymLink));

    if(auto pkg = findPackage(info.name); pkg)
        throw std::runtime_error(std::format("Package '{}' already exists.", info.name));

    // if(!pkg->didOutputExists())
        // fs::create_directory(output);
    // else
        // WARNING(utils::fmt_to_str("Directory '%s' exists, it'll not removed.", output.c_str()));

    // for(auto entry : fs::directory_iterator(input)) {
        // auto relativePath = fs::relative(entry.path(), input);
        // auto outPath = output/relativePath;

        // writePathInCacheFile(cacheFile, relativePath);
        // if(symlink)
          // copySymLinkDirectory(entry, outPath);
        // else
          // copyDirectory(entry, outPath);
        

        // VERBOSE(utils::fmt_to_str("'%s' copied ...", entry.path().c_str()));
        // pkg->addRelativePath(relativePath);
    // }

    // VERBOSE(utils::fmt_to_str("Package '%s' saved into cache.", pkg->name().c_str()));
    // m_pkgs.push_back(pkg);
    // return *pkg;
    
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
  /*
  PackageManager::~PackageManager() {
    for(auto& pkg : m_pkgs)
        delete pkg;
    m_pkgs.clear();
  }

  Package& PackageManager::createPackageFromDirectory(const fs::path& input, const fs::path& output, bool symlink, const char* nameOrNull) {
    std::string name;
    if(nameOrNull == nullptr)
        name = input.stem().string();
    else
        name = nameOrNull;

    Package* pkg = findPackage(name);
    if(pkg)
        throw PackageException(utils::fmt_to_str("Package '%s' already exists.", name.c_str()));
    
    pkg = new Package(output, name, fs::is_directory(output));
    utils::WFile cacheFile(createCacheFileFromPackage(*pkg));

    if(!pkg->didOutputExists())
        fs::create_directory(output);
    else
        WARNING(utils::fmt_to_str("Directory '%s' exists, it'll not removed.", output.c_str()));

    for(auto entry : fs::directory_iterator(input)) {
        auto relativePath = fs::relative(entry.path(), input);
        auto outPath = output/relativePath;

        writePathInCacheFile(cacheFile, relativePath);
        if(symlink)
          copySymLinkDirectory(entry, outPath);
        else
          copyDirectory(entry, outPath);
        

        VERBOSE(utils::fmt_to_str("'%s' copied ...", entry.path().c_str()));
        pkg->addRelativePath(relativePath);
    }

    VERBOSE(utils::fmt_to_str("Package '%s' saved into cache.", pkg->name().c_str()));
    m_pkgs.push_back(pkg);
    return *pkg;
  }

  void PackageManager::copyDirectory(const fs::directory_entry& e, const fs::path& output) {
    fs::copy(
        e.path(),
        output,
        fs::copy_options::recursive
    );
  }

  void PackageManager::copySymLinkDirectory(const fs::directory_entry& e, const fs::path& output) {
    if(e.is_directory())
        fs::create_directory_symlink(e.path(), output);
    else
        fs::create_symlink(e.path(), output);
  }

  void PackageManager::uninstallPackage(const std::string& name) {
    Package* pkg = findPackage(name);
    if(pkg == nullptr)
        throw PackageException(utils::fmt_to_str("Package '%s' don't exists.", name.c_str()));

    for(auto absPath : *pkg) {
        fs::remove_all(absPath);
        VERBOSE(utils::fmt_to_str("'%s' removed successfully ...", absPath.c_str()));
    }

    if(!pkg->didOutputExists()) {
        std::error_code e;
        fs::remove(pkg->outDir(), e);
        if(e)
          WARNING(utils::fmt_to_str("Output directory '%s' will not removed: %s", pkg->outDir().c_str(), e.message().c_str()));
        else
          VERBOSE(utils::fmt_to_str("Output directory '%s' removed successfully.", pkg->outDir().c_str()));
    }

    removePackage(pkg);
    INFO(utils::fmt_to_str("Uninstallation '%s' successfully!", name.c_str()));
  }

  const Package* PackageManager::findPackage(const std::string& name) const {
    auto it = std::find_if(m_pkgs.cbegin(), m_pkgs.cend(), same_package(name));
    if(it != m_pkgs.cend())
        return *it;
    
    return nullptr;
  }
  Package* PackageManager::findPackage(const std::string& name) {
    auto it = std::find_if(m_pkgs.begin(), m_pkgs.end(), same_package(name));
    if(it != m_pkgs.end())
        return *it;
    
    return nullptr;
  }

  bool PackageManager::hasPackage(const std::string& name) const {
    return findPackage(name) != nullptr;
  }

  void PackageManager::removePackage(const Package* pkg) {
    for(auto it = m_pkgs.begin(); it != m_pkgs.end(); ++it) {
        if(*it == pkg) {
          fs::remove(m_dir/(*it)->name());
          m_pkgs.erase(it);
          VERBOSE(utils::fmt_to_str("Package '%s' deleted successfully!", pkg->name().c_str()));
          break;
        }
    }
  }
  */

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