#pragma once
#include "fs.hpp"
#include "package_info.hpp"
#include "thread_pool.hpp"

#include <vector>
#include <future>

namespace app {
  class Package {
  public:
    Package(const PackageInfo& info, std::vector<fs::path>&& paths);

    std::future<bool> save();

    const PackageInfo& info() const {return m_info;}
  private:
    ThreadPool m_pool;
    PackageInfo m_info;
    std::mutex m_mtx;
    std::promise<bool> m_promise;
    size_t m_completed{};
    std::vector<fs::path> m_paths;
  };
} // namespace app