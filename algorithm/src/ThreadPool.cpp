
#include <algorithm>
#include "ThreadPool.hpp"


ThreadPool::ThreadPool(uint32_t num_threads) {
  threads.reserve(num_threads);
  for (uint32_t i = 0; i < num_threads; ++i) {
    threads.emplace_back(&ThreadPool::run, this);
  }
}


ThreadPool::ThreadPool(ThreadPool const& src)
{
  threads.reserve(src.threads.size());
  for (uint32_t i = 0; i < threads.size(); ++i) {
    threads.emplace_back(&ThreadPool::run, this);
  }
}


ThreadPool::~ThreadPool() {
  quite = true;
  for (uint32_t i = 0; i < threads.size(); ++i) {
    q_cv.notify_all();
    threads[i].join();
  }
}


void ThreadPool::wait(int64_t task_id) {
  std::unique_lock<std::mutex> lock(completed_task_ids_mtx);
  completed_task_ids_cv.wait(lock, [this, task_id]()->bool {
      return completed_task_ids.find(task_id) != completed_task_ids.end();
      });
}


void ThreadPool::wait_all() {
  std::unique_lock<std::mutex> lock(q_mtx);

  completed_task_ids_cv.wait(lock, [this]()->bool {
      std::lock_guard<std::mutex> task_lock(completed_task_ids_mtx);
      return q.empty() && last_idx == (int)completed_task_ids.size();
      });
}


bool ThreadPool::calculated(int64_t task_id) {

  std::lock_guard<std::mutex> lock(completed_task_ids_mtx);

  if (completed_task_ids.find(task_id) != completed_task_ids.end()) {
    return true;
  }
  return false;
}


void ThreadPool::run() {

  while (!quite) {

    std::unique_lock<std::mutex> lock(q_mtx);

    q_cv.wait(lock, [this]()->bool { return !q.empty() || quite; });

    if (!q.empty()) {

      auto elem = std::move(q.front());
      q.pop();
      lock.unlock();

      elem.first.get();

      std::lock_guard<std::mutex> lock(completed_task_ids_mtx);
      completed_task_ids.insert(elem.second);

      completed_task_ids_cv.notify_all();
    }
  }
}