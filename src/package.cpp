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
    m_pool.start(info().nthreads);
    for(fs::path path; hasNextFile();) {
      path = nextFile();
      ++m_nfiles;
      m_pool.queueJob([path = std::move(path), this] {
        if(m_skip) return;
        if(!processFile(path)) {
          m_promise.set_value(false);
          m_skip = true;
          return;
        }

        std::unique_lock lock(m_mtx);
        ++m_nfilesProcessed;
        VERBOSE("File '{}' processed successfully", path.string());
        if(m_nfiles <= m_nfilesProcessed) {
          m_promise.set_value(true);
          m_skip = true;
        }
      });
    }

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

  fs::path InstallPackage::nextFile() {
    return *m_it++;    
  }

  bool InstallPackage::hasNextFile() {
    return m_it != fs::recursive_directory_iterator();
  }

  bool InstallPackage::processFile(const fs::path& filepath) {
    {
    std::unique_lock lock(m_mtx);
    m_cachefile.writePath(filepath);
    m_cachefile.flush();
    }
    return true;
  }

  UninstallPackage::UninstallPackage(const fs::path& cachepath) :
    m_cachefile{cachepath} {
    char buf[6];
    m_cachefile.read(buf, 6);
    if(strcmp(buf, Package::MAGIC_NUMBER) != 0) {
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

  fs::path UninstallPackage::nextFile() {
    fs::path res;
    m_cachefile.readPath(&res);
    return res;
  }

  bool UninstallPackage::hasNextFile() {
    return !m_cachefile.isAtEnd();
  }

  bool UninstallPackage::processFile(const fs::path& filepath) {
    return true;
  }

  void UninstallPackage::onEndProcess() {
    auto f = m_cachefile.filepath();
    m_cachefile.close();
    fs::remove(f);
  }
} // namespace app