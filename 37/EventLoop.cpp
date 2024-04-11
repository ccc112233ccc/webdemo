#include "EventLoop.h"

#include <spdlog/spdlog.h>

#include "Channel.h"

int creattimerfd(int sec = 5, int nsec = 0) {
  int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (timerfd < 0) {
    perror("timerfd_create");
  }
  itimerspec new_value;
  bzero(&new_value, sizeof(new_value));
  new_value.it_value.tv_sec = sec;
  new_value.it_value.tv_nsec = nsec;
  timerfd_settime(timerfd, 0, &new_value, NULL);
  return timerfd;
}

EventLoop::EventLoop(bool mainloop, int timetvl, int timerout)
    : mainloop_(mainloop),
      timetvl_(timetvl),
      timerout_(timerout),
      ep_(new Epoll()),
      wakeup_fd_(eventfd(0, EFD_NONBLOCK)),
      wakeup_channel_(new Channel(this, wakeup_fd_)),
      timer_fd_(creattimerfd(timetvl, 0)),
      timer_channel_(new Channel(this, timer_fd_)) {
  wakeup_channel_->set_read_callback(
      std::bind(&EventLoop::handle_wakeup, this));
  wakeup_channel_->enable_reading();

  timer_channel_->set_read_callback(
      std::bind(&EventLoop::handle_timeout, this));
  timer_channel_->enable_reading();
}
EventLoop::~EventLoop() {}

void EventLoop::loop() {
  // printf("EventLoop::loop thread id (%ld)\n", syscall(SYS_gettid));
  thread_id_ = syscall(SYS_gettid);
  while (stop_ == false) {
    std::vector<Channel*> channels = ep_->wait(-1);
    if (channels.empty()) {
      timeout_callback_(this);
    }
    for (auto& ch : channels) {
      ch->handle_events();
    }
  }
}

void EventLoop::stop() {
  stop_ = true;
  wakeup();
}

void EventLoop::update_channel(Channel* channel) {
  ep_->update_channel(channel);
}

void EventLoop::remove_channel(Channel* channel) {
  ep_->remove_channel(channel);
}

void EventLoop::queue_in_loop(std::function<void()> cb) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    taskqueue_.push(cb);
  }

  wakeup();
}

void EventLoop::wakeup() {
  uint64_t one = 1;
  ssize_t n = write(wakeup_fd_, &one, sizeof(one));
  if (n != sizeof(one)) {
    perror("EventLoop::wakeup write");
  }
}

void EventLoop::handle_wakeup() {
  // printf("EventLoop::handle_wakeup\n");
  uint64_t one;
  ssize_t n = read(wakeup_fd_, &one, sizeof(one));
  if (n != sizeof(one)) {
    perror("EventLoop::handle_wakeup read");
  }
  std::lock_guard<std::mutex> lock(mutex_);
  while (!taskqueue_.empty()) {
    auto cb = taskqueue_.front();
    taskqueue_.pop();
    cb();
  }
}

void EventLoop::handle_timeout() {
  itimerspec new_value;
  bzero(&new_value, sizeof(new_value));
  new_value.it_value.tv_sec = timetvl_;
  new_value.it_value.tv_nsec = 0;
  timerfd_settime(timer_fd_, 0, &new_value, NULL);
  if (mainloop_) {
    // printf("mainloop_ is true\n");
  } else {
    // printf("thread id (%ld). ", syscall(SYS_gettid));
    // 将超时的连接从 connections_ 中删除，
    // 使用迭代器遍历的时候可能会有新的连接加入，所以需要加锁
    std::lock_guard<std::mutex> lock(conn_mutex_);
    for (auto it = connections_.begin(); it != connections_.end();) {
      if (it->second->timedout(timerout_)) {
        spdlog::debug("connection timeout(fd = {}, ip = {}, port = {})",
                      it->second->fd(), it->second->ip(), it->second->port());
        connection_timeout_cb_(it->first);
        it = connections_.erase(it);
      } else {
        it++;
      }
    }
  }
}

void EventLoop::add_connection(spConnection conn) {
  std::lock_guard<std::mutex> lock(conn_mutex_);
  connections_[conn->fd()] = conn;
}

void EventLoop::delete_connection(spConnection conn) {
  std::lock_guard<std::mutex> lock(conn_mutex_);
  connections_.erase(conn->fd());
}