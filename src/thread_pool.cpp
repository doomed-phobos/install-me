#include "thread_pool.hpp"

namespace app {
  ThreadPool::~ThreadPool() {
    stop();
  }

  void ThreadPool::start(unsigned nthreads) {
    for(unsigned i = 0; i < nthreads; ++i)
      m_threads.emplace_back(std::thread(&ThreadPool::loop, this));
  }

  void ThreadPool::loop() {
    while(true) {
      std::function<void()> job;
      {
        std::unique_lock lock(m_queueMtx);
        m_mtxCondition.wait(lock, [this] {return !m_jobs.empty() || m_shouldTerminate;});
        if(m_shouldTerminate)
          return;
        job = m_jobs.front();
        m_jobs.pop();
      }
      job();
    }
  }

  void ThreadPool::queueJob(std::function<void()>&& fn) {
    {
      std::unique_lock lock(m_queueMtx);
      m_jobs.push(std::move(fn));
    }
    m_mtxCondition.notify_one();
  }

  bool ThreadPool::isBusy() {
    bool res{};
    {
      std::unique_lock lock(m_queueMtx);
      res = !m_jobs.empty();
    }

    return res;
  }

  void ThreadPool::stop() {
    {
      std::unique_lock lock(m_queueMtx);
      m_shouldTerminate = true;
    }
    m_mtxCondition.notify_all();
    for(auto& t : m_threads)
      t.join();
    m_threads.clear();
  }
} // namespace app