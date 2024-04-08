#include "EventLoop.h"

#include "Channel.h"

int creattimerfd(int sec = 30, int nsec = 0) {
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

EventLoop::EventLoop(bool mainloop)
    : mainloop_(mainloop),
      ep_(new Epoll()),
      wakeup_fd_(eventfd(0, EFD_NONBLOCK)),
      wakeup_channel_(new Channel(this, wakeup_fd_)),
      timer_fd_(creattimerfd()),
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
  printf("EventLoop::loop thread id (%ld)\n", syscall(SYS_gettid));
  thread_id_ = syscall(SYS_gettid);
  while (true) {
    std::vector<Channel*> channels = ep_->wait(-1);
    if (channels.empty()) {
      timeout_callback_(this);
    }
    for (auto& ch : channels) {
      ch->handle_events();
    }
  }
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
  printf("EventLoop::handle_wakeup\n");
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
  new_value.it_value.tv_sec = 5;
  new_value.it_value.tv_nsec = 0;
  timerfd_settime(timer_fd_, 0, &new_value, NULL);
  if (mainloop_) {
    printf("mainloop_ is true\n");
  } else {
    printf("mainloop_ is false\n");
  }
}