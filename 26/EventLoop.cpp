#include "EventLoop.h"

#include <sys/syscall.h>
#include <unistd.h>

#include "Channel.h"

EventLoop::EventLoop() : ep_(new Epoll()) {}
EventLoop::~EventLoop() {}

void EventLoop::loop() {
  printf("EventLoop::loop thread id (%ld)\n", syscall(SYS_gettid));
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