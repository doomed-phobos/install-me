#pragma once
#include <condition_variable>
#include <mutex>
#include <functional>
#include <queue>
#include <vector>

namespace app {
  class ThreadPool {
  public:
    ~ThreadPool();
    void start(unsigned nthreads);
    void queueJob(std::function<void()>&& fn);
    void stop();
    bool isBusy();
  private:
    void loop();

    bool m_shouldTerminate{};
    std::mutex m_queueMtx;
    std::condition_variable m_mtxCondition;
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_jobs;
  };
} // namespace app