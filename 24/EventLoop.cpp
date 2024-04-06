#include "EventLoop.h"

#include "Channel.h"

EventLoop::EventLoop() : ep_(new Epoll()) {}
EventLoop::~EventLoop() {}

void EventLoop::loop() {
  while (true) {
    std::vector<Channel*> channels = ep_->wait(10 * 1000);
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