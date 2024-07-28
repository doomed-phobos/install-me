#include "package.hpp"

#include "output.hpp"

#include <thread>

using namespace std::chrono_literals;

namespace app {
  Package::Package(const PackageInfo& info, std::vector<fs::path>&& paths) :
    m_info{info},
    m_paths{std::move(paths)} {}

  std::future<bool> Package::save() {
    VERBOSE("Using {} threads of {}", m_info.nthreads, std::thread::hardware_concurrency());

    std::future<bool> future = m_promise.get_future();
    for(const auto& path : m_paths) {
      m_pool.queueJob([this] {
        std::this_thread::sleep_for(1s);
        std::unique_lock lock(m_mtx);
        ++m_completed;
        if(m_completed == m_paths.size())
          m_promise.set_value(true);
        INFO("Copied {} of {}", m_completed, m_paths.size());
      });
    }

    m_pool.start(m_info.nthreads);
    while(m_pool.isBusy());

    return future;
  }
} // namespace app