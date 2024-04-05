#include "EventLoop.h"

#include "Channel.h"

EventLoop::EventLoop() : ep_() {}
EventLoop::~EventLoop() {}

void EventLoop::loop() {
  while (true) {
    std::vector<Channel*> channels = ep_.wait();
    for (auto& ch : channels) {
      ch->handle_events();
    }
  }
}