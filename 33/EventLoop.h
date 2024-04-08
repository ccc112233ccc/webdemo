#pragma once
#include <sys/eventfd.h>
#include <sys/syscall.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include <functional>
#include <memory>
#include <mutex>
#include <queue>

#include "Epoll.h"

class EventLoop {
 private:
  std::unique_ptr<Epoll> ep_;
  std::function<void(EventLoop*)> timeout_callback_;
  pid_t thread_id_;
  std::queue<std::function<void()>> taskqueue_;
  std::mutex mutex_;
  int wakeup_fd_;
  std::unique_ptr<Channel> wakeup_channel_;
  int timer_fd_;
  std::unique_ptr<Channel> timer_channel_;
  bool mainloop_;

 public:
  EventLoop(bool mainloop = false);
  ~EventLoop();
  void loop();
  void remove_channel(Channel* channel);
  void update_channel(Channel* channel);
  void set_timeout_callback(std::function<void(EventLoop*)> cb) {
    timeout_callback_ = cb;
  }
  bool in_loop_thread() { return thread_id_ == syscall(SYS_gettid); }
  void queue_in_loop(std::function<void()> cb);
  void wakeup();
  void handle_wakeup();
  void handle_timeout();
};