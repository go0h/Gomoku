
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
  ~ThreadPool();

  template <typename Func, typename ...Args>
  int64_t add_task(const Func& task_func, Args&&... args);

  void wait(int64_t task_id);
  void wait_all();

  bool calculated(int64_t task_id);

private:

  void run();

  std::queue<std::pair<std::future<void>, int64_t>> q; // очередь задач - хранит функцию(задачу), которую нужно исполнить и номер данной задачи
  std::mutex q_mtx;
  std::condition_variable q_cv;

  std::unordered_set<int64_t> completed_task_ids;      // помещаем в данный контейнер исполненные задачи
  std::condition_variable completed_task_ids_cv;
  std::mutex completed_task_ids_mtx;

  std::vector<std::thread> threads;

  std::atomic<bool> quite{ false };                    // флаг завершения работы thread_pool
  std::atomic_int last_idx{0};                         // переменная хранящая id который будет выдан следующей задаче
};

