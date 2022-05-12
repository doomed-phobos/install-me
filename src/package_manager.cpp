#include "src/package_manager.hpp"

#include "src/generate_macros.hpp"
#include "src/fs.hpp"
#include "src/package.hpp"
#include "src/file.hpp"
#include "src/output.hpp"

#include <climits>
#include <cstring>
#include <algorithm>

namespace {
   std::string get_cache_home_dir() {
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

   struct same_package {
      same_package(const std::string& name) :
         m_name(name) {}
      
      bool operator()(const app::Package& pkg) const {
         return pkg.pkgName() == m_name;
      }

      std::string m_name;
   };
}

namespace app {
   PackageManager::PackageManager(const fs::path& dir) :
      m_dir(dir) {
      for(auto path : fs::directory_iterator(m_dir)) {
         if(!path.is_directory()) {
            // Check if it's a cache
            auto fPath = path.path();
            utils::File file(fPath);
            char buf[6];
            file.read(buf, 6);
            if(strcmp(buf, MAGIC_NUMBER) == 0)
               m_pkgs.push_back(loadPackage(fPath.filename(), file));
         }
      }
   }

   Package PackageManager::loadPackage(const std::string& pkgName, utils::File& file) {
      uint32_t lengthPath;
      file.readU32(&lengthPath);

      std::vector<char> outputDir(lengthPath+1);
      file.read(&outputDir[0], lengthPath);

      std::string str(outputDir.data());
      Package pkg(std::string(outputDir.data()), pkgName);
      while(!file.isAtEnd()) {
         file.readU32(&lengthPath);
         
         std::vector<char> relativePath(lengthPath+1);
         file.read(&relativePath[0], lengthPath);
         pkg.addPath(std::string(relativePath.data()));
      }
      return pkg;
   }

   bool PackageManager::savePackage(Package&& pkg) {
      utils::WFile file(m_dir/pkg.pkgName());
      // Put magic number
      file.writeText(MAGIC_NUMBER);
      // Put output dir
      std::string outputDir = pkg.outDir().string();
      file.write32(outputDir.size());
      file.writeText(outputDir.c_str());
      // Put relative paths
      for(const auto& path : pkg.relativePaths()) {
         std::string strPath = path.string();
         file.write32(strPath.size());
         file.writeText(strPath.c_str());
      }

      m_pkgs.push_back(std::move(pkg));

      return true;
   }

   const Package* PackageManager::findPackage(const std::string& name) const {
      auto it = std::find_if(m_pkgs.begin(), m_pkgs.end(), same_package(name));
      if(it != m_pkgs.end())
         return &(*it);
      
      return nullptr;
   }
   Package* PackageManager::findPackage(const std::string& name) {
      return const_cast<Package*>(findPackage(name));
   }

   bool PackageManager::hasPackage(const std::string& name) const {
      return findPackage(name) != nullptr;
   }

   const PackageManager::PackageList& PackageManager::pkgs() const {return m_pkgs;}

   PackageManager* PackageManager::GetInstance() {
      if(!instance)
         instance.reset(new PackageManager(CreateAndGetCacheDirectory()));
      
      return instance.get();
   }

   fs::path PackageManager::CreateAndGetCacheDirectory() {
      fs::path cacheDir = get_cache_home_dir();
      if(!fs::exists(cacheDir)) {
         std::error_code e;
         fs::create_directories(cacheDir/cache_mgr_dirname, e);
         if(e)
            throw std::runtime_error("Package directory: " + e.message());
      }

      return cacheDir/cache_mgr_dirname;
   }

   std::ostream& operator<<(std::ostream& out, const PackageManager& pkgManager) {
      utils::table pkgs("PACKAGE LIST");
      utils::table::column name(pkgs, "Name");
      utils::table::column id(pkgs, "Installation directory");
      for(const auto& pkg : pkgManager.pkgs()) {
         name.addItem(pkg.pkgName());
         id.addItem(pkg.outDir().string());
      }

      pkgs.addColumn(std::move(name));
      pkgs.addColumn(std::move(id));

      return out << pkgs;
   }
} // namespace app