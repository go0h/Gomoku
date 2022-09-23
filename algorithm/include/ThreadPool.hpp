
#include <iostream>
#include <queue>
#include <thread>
#include <chrono>
#include <mutex>
#include <future>
#include <unordered_set>
#include <functional>
#include <atomic>

#include <vector>
#include <chrono>


class ThreadPool {

public:

  ThreadPool(uint32_t num_threads);
  ThreadPool(ThreadPool const & src);
  ~ThreadPool();

  template <typename Func, typename ...Args>
  int64_t add_task(const Func& task_func, Args&&... args) {

    int64_t task_idx = last_idx++;

    std::lock_guard<std::mutex> q_lock(q_mtx);
    q.emplace(std::async(std::launch::deferred, task_func, args...), task_idx);
    q_cv.notify_one();
    return task_idx;
  }

  void wait(int64_t task_id);
  void wait_all();

  bool calculated(int64_t task_id);

private:

  void run();

  // очередь задач - хранит функцию(задачу), которую нужно исполнить и номер данной задачи
  std::queue<std::pair<std::future<void>, int64_t>> q;
  std::mutex                                        q_mtx;
  std::condition_variable                           q_cv;

  // помещаем в данный контейнер исполненные задачи
  std::unordered_set<int64_t> completed_task_ids;
  std::condition_variable     completed_task_ids_cv;
  std::mutex                  completed_task_ids_mtx;

  std::vector<std::thread>    threads;

  // флаг завершения работы thread_pool
  std::atomic_bool            quite{false};

  // переменная хранящая id который будет выдан следующей задаче
  std::atomic_int             last_idx{0};
};

