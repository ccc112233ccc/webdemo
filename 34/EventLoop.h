#pragma once
#include <sys/eventfd.h>
#include <sys/syscall.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <queue>

#include "Connection.h"
#include "Epoll.h"
class Channel;
class Connection;
using spConnection = std::shared_ptr<Connection>;

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
  std::mutex conn_mutex_;
  std::map<int, spConnection> connections_;
  std::function<void(int)> connection_timeout_cb_;

  int timetvl_;   // 闹钟时间间隔
  int timerout_;  // 连接超时时间

 public:
  EventLoop(bool mainloop = false, int timetvl = 30, int timerout = 80);
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
  void add_connection(spConnection conn);
  void set_connection_timeout_cb(std::function<void(int)> cb) {
    connection_timeout_cb_ = cb;
  }
};