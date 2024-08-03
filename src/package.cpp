#include "package.hpp"

#include "output.hpp"
#include "file.hpp"
#include "package_manager.hpp"

#include <cstring>
#include <thread>

using namespace std::chrono_literals;
// TODO: Crear una coroutine que devuelve un path y manajerlo, tal vez podamos crear ese coroutine con una function virtual
namespace app {
  Package::Package(const PackageInfo& info) :
    m_info{info} {}

  InstallPackage::InstallPackage(const PackageInfo& info) :
    Package{info},
    m_it{info.inputDir} {}

  bool Package::operator()(const fs::path& cachedir) {
    if(isCompleted()) return false;

    onStartProcess();
    bool res = asyncProcess().get();
    if(res) completed();
    return res;
  }

  std::future<bool> Package::asyncProcess() {
    INFO("Using {} threads of {}", info().nthreads, std::thread::hardware_concurrency());
    if(info().nthreads > std::thread::hardware_concurrency())
      WARNING("You're using more threads that availables!!");

    std::future<bool> future = m_promise.get_future();
    while(hasNextPath()) {
      auto path = nextPath();
      if(fs::is_directory(path)) {
        if(!processDirectory(path)) {
          m_promise.set_value(false);
          return future;
        }
        continue;
      }

      m_pool.queueJob([path = std::move(path), this] {
        if(m_shouldStop) return;
        
        if(!processFile(path)) {
          m_promise.set_value(false);
          m_shouldStop = true;
          return;
        }

        {
        std::unique_lock lock(m_mtx);
        ++m_nfilesProcessed;
        VERBOSE("File '{}' processed successfully. {} of {}", path.string(), m_nfilesProcessed, m_nfiles);
        if(m_nfiles <= m_nfilesProcessed) {
          m_promise.set_value(true);
        }
        }
      });
      ++m_nfiles;
    }
    
    m_pool.start(info().nthreads);

    return future;
  }

  void InstallPackage::onStartProcess() {
    createCacheFile(PackageManager::GetInstance().cacheDir()/info().name);
  }

  void InstallPackage::createCacheFile(const fs::path& cachepath) {
    m_cachefile.open(cachepath);
    m_cachefile.writeText(MAGIC_NUMBER);
    m_cachefile.write32(info().flags.value());
    m_cachefile.writePath(info().inputDir);
    m_cachefile.writePath(info().outputDir);
    m_cachefile.flush();
  }

  fs::path InstallPackage::nextPath() {                                           
    return *m_it++;
  }

  bool InstallPackage::hasNextPath() {
    return m_it != fs::recursive_directory_iterator();
  }

  bool InstallPackage::processDirectory(const fs::path& dirpath) {
    const auto& output = m_info.outputDir / fs::relative(dirpath, m_info.inputDir);
    VERBOSE("Creating directory in '{}'...", output.string());
    std::error_code e;
    if(!fs::create_directory(output, e)) {
      if(e) {
        ERROR("Failed to create directory in '{}'", output.string());
        return false;
      }

      WARNING("Directory located in '{}' already exists", output.string());
    } else {
      std::unique_lock lock(m_mtx);
      m_cachefile.writePath(output);
      m_cachefile.flush();
    }

    return true;
  }

  bool InstallPackage::processFile(const fs::path& filepath) {
    VERBOSE("Copying file from '{}' to '{}'...", filepath.string(), m_info.outputDir.string());
    const auto& output = m_info.outputDir / fs::relative(filepath, m_info.inputDir);
    std::error_code e;

    fs::copy(filepath, output, e);
    if(e) goto err;

    {
    std::unique_lock lock(m_mtx);
    m_cachefile.writePath(output);
    m_cachefile.flush();
    }

    return true;

    err:
      ERROR("Failed to copy '{}': {}", filepath.string(), e.message());
      return false;
  }

  UninstallPackage::UninstallPackage(const fs::path& cachepath) :
    m_cachefile{cachepath} {
    char buf[6];
    m_cachefile.read(buf, 6);
    if(strncmp(buf, Package::MAGIC_NUMBER, sizeof(buf)) != 0) {
      WARNING("File '{}' isn't a cache", m_cachefile.filepath().string());
      return; 
    }
    
    unsigned flags;
    m_cachefile.readU32(&flags);
    m_info.flags = flags;
    m_info.name = m_cachefile.filepath().filename();
    m_cachefile.readPath(&m_info.inputDir);
    m_cachefile.readPath(&m_info.outputDir);
  }

  fs::path UninstallPackage::nextPath() {
    fs::path res;
    m_cachefile.readPath(&res);
    return res;
  }

  bool UninstallPackage::hasNextPath() {
    return !m_cachefile.isAtEnd();
  }

  bool UninstallPackage::processFile(const fs::path& filepath) {
    VERBOSE("Removing '{}' file...", filepath.string());
    std::error_code e;
    fs::remove(filepath, e);
    if(e) {
      ERROR("Failed to uninstall '{}': {}", filepath.string(), e.message());
      return false;
    }
    return true;
  }

  bool UninstallPackage::processDirectory(const fs::path& dirpath) {
    m_disposeDirs.push(dirpath);
    return true;
  }

  void UninstallPackage::onEndProcess() {
    auto f = m_cachefile.filepath();
    m_cachefile.close();

    for(fs::path p; !m_disposeDirs.empty(); m_disposeDirs.pop()) {
      p = m_disposeDirs.top();
      VERBOSE("Deleting directory located in '{}'", p.string());
      if(std::error_code e; fs::remove(p, e), e) {
        WARNING("Failed to delete directory '{}': {}", p.string(), e.message());
      }
    }

    if(std::error_code e; !fs::remove(m_info.outputDir, e))
      WARNING("Directory '{}' isn't empty", m_info.outputDir.string());
    fs::remove(f);
  }
} // namespace app