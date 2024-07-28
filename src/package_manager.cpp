#include "package_manager.hpp"

#include "generate_macros.hpp"
#include "fs.hpp"
#include "package.hpp"
#include "file.hpp"
#include "output.hpp"
#include "package_exception.hpp"
#include "string.hpp"

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
      
      bool operator()(const app::Package* pkg) const {
         return pkg->name() == m_name;
      }

      std::string m_name;
   };
}

namespace app {
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

   PackageManager::~PackageManager() {
      for(auto& pkg : m_pkgs)
         delete pkg;
      m_pkgs.clear();
   }

   Package* PackageManager::loadPackage(const fs::path& filepath) {
      utils::File file(filepath);

      char buf[6];
      file.read(buf, 6);
      if(strcmp(buf, MAGIC_NUMBER) != 0)
         return nullptr;
      
      uint8_t didOutputExists;
      file.readU8(&didOutputExists);

      uint32_t lengthPath;
      file.readU32(&lengthPath);

      std::vector<char> outputDir(lengthPath+1);
      file.read(&outputDir[0], lengthPath);

      std::string name = filepath.filename();
      Package* pkg = new Package(fs::path(outputDir.data()), name, didOutputExists);
      while(!file.isAtEnd()) {
         file.readU32(&lengthPath);
         
         std::vector<char> relativePath(lengthPath+1);
         file.read(&relativePath[0], lengthPath);
         pkg->addRelativePath(std::string(relativePath.data()));
      }

      return pkg;
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

   utils::WFile PackageManager::createCacheFileFromPackage(const Package& pkg) {
      utils::WFile file(m_dir/pkg.name());
      // Put magic number
      file.writeText(MAGIC_NUMBER);
      // Put options
      file.write8(pkg.didOutputExists());
      // Put output dir
      std::string outputDir = pkg.outDir().string();
      file.write32(outputDir.size());
      file.writeText(outputDir.c_str());
      file.flush();

      return std::move(file);
   }

   void PackageManager::writePathInCacheFile(utils::WFile& cacheFile, const fs::path& path) {
      std::string strPath = path.string();
      cacheFile.write32(strPath.size());
      cacheFile.writeText(strPath.c_str());
      cacheFile.flush();
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

   const PackageManager::PackageList& PackageManager::pkgs() const {return m_pkgs;}
   const fs::path& PackageManager::cacheDir() const {return m_dir;}

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
            throw PackageException("Package directory: " + e.message());
      }

      return cacheDir/cache_mgr_dirname;
   }

   std::ostream& operator<<(std::ostream& out, const PackageManager& pkgManager) {
      utils::table pkgs("PACKAGE LIST");
      utils::table::column name(pkgs, "Name");
      utils::table::column path(pkgs, "Path");
      utils::table::column id(pkgs, "Installation directory");
      for(const auto& pkg : pkgManager.pkgs()) {
         name.addItem(pkg->name());
         path.addItem((pkgManager.cacheDir()/pkg->name()).string());
         id.addItem(pkg->outDir().string());
      }

      pkgs.addColumn(std::move(name));
      pkgs.addColumn(std::move(path));
      pkgs.addColumn(std::move(id));

      return out << pkgs;
   }
} // namespace app