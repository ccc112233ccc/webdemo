#pragma once

#include <spdlog/spdlog.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
 private:
  std::vector<std::thread> threads;
  std::queue<std::function<void()>> tasks;

  std::mutex m;
  std::condition_variable cv;
  std::atomic<bool> stop_{false};
  std::string name_;

 public:
  ThreadPool(size_t n, const std::string& name = "ThreadPool") : name_(name) {
    for (size_t i = 0; i < n; i++) {
      threads.emplace_back([this] {
        spdlog::info("{} thread create({})", name_, syscall(SYS_gettid));
        while (true) {
          std::unique_lock<std::mutex> lock(m);
          cv.wait(lock, [this] { return stop_ || !tasks.empty(); });
          if (stop_ && tasks.empty()) {
            return;
          }

          auto task = std::move(tasks.front());
          tasks.pop();
          lock.unlock();
          spdlog::debug("{} thread({}) start task", name_, syscall(SYS_gettid));
          task();
        }
      });
    }
  }
  ~ThreadPool() { stop(); }

  template <class F, class... Args>
  void enqueue(F&& f, Args&&... args) {
    std::unique_lock<std::mutex> lock(m);
    tasks.emplace([=] { f(args...); });
    lock.unlock();
    cv.notify_one();
  }

  void add_task(std::function<void()> task) {
    std::unique_lock<std::mutex> lock(m);
    tasks.emplace(task);
    lock.unlock();
    cv.notify_one();
  }

  size_t size() { return threads.size(); }

  void stop() {
    if (stop_) {
      return;
    }
    std::unique_lock<std::mutex> lock(m);
    stop_ = true;
    lock.unlock();
    cv.notify_all();
    for (auto& t : threads) {
      t.join();
    }
  }
};