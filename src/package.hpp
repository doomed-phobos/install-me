#pragma once
#include "fs.hpp"
#include "file.hpp"
#include "package_info.hpp"
#include "thread_pool.hpp"

#include <queue>
#include <future>

namespace app {
  class Package {
  public:
    static constexpr const char* MAGIC_NUMBER = "lCache";
    virtual ~Package() {}

    bool isCompleted() const {return m_completed;}
    bool operator()(const fs::path& cachedir);

    const PackageInfo& info() const {return m_info;}
  protected:
    Package(const PackageInfo& info);
    Package() {}

    /// Iterator
    virtual fs::path nextPath() = 0;
    virtual bool hasNextPath() = 0;

    std::future<bool> asyncProcess();
    void completed() {onEndProcess(); m_completed = true;}

    virtual void onStartProcess() {}
    virtual bool processFile(const fs::path& filepath) = 0;
    virtual bool processDirectory(const fs::path& dirpath) = 0;
    virtual void onEndProcess() {}
    
    PackageInfo m_info;
  private:
    ThreadPool m_pool;
    std::mutex m_mtx;
    std::promise<bool> m_promise;
    size_t m_nfiles{};
    size_t m_nfilesProcessed{};
    bool m_completed{};
  };

  class InstallPackage : public Package {
  public:
    InstallPackage(const PackageInfo& info);
  private:
    virtual fs::path nextPath() override;
    virtual bool hasNextPath() override;

    virtual void onStartProcess() override;
    virtual bool processFile(const fs::path& filepath) override;
    virtual bool processDirectory(const fs::path& dirpath) override;
    void createCacheFile(const fs::path& filepath);

    std::mutex m_mtx;
    utils::WFile m_cachefile;
    fs::recursive_directory_iterator m_it;
  };

  class UninstallPackage : public Package {
  public:
    UninstallPackage(const fs::path& cachepath);
  private:
    virtual fs::path nextPath() override;
    virtual bool hasNextPath() override;

    virtual bool processFile(const fs::path& filepath) override;
    virtual bool processDirectory(const fs::path& dirpath) override;
    virtual void onEndProcess() override;

    std::queue<fs::path> m_dirs;
    utils::File m_cachefile;
  };
} // namespace app