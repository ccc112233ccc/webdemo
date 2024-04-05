#pragma once
#include <sys/epoll.h>

#include <functional>

class Epoll;
class Socket;
class EventLoop;

class Channel {
 private:
  int fd_;
  uint32_t events_;
  uint32_t revents_;

  bool inepoll_;
  std::function<void()> read_callback_;
  EventLoop* loop_;

 public:
  Channel(EventLoop* loop_, int fd);
  ~Channel();

  int fd();               // getter
  void useet();           // 边缘触发
  void enable_reading();  // 开启读事件
  void setin_epoll();     // 设置在epoll中
  void set_revents(uint32_t revents);
  bool is_in_epoll();

  uint32_t events();   // getter
  uint32_t revents();  // getter

  void handle_events();
  // void new_connect(Socket* servsock);
  void on_message();
  void set_read_callback(std::function<void()> cb) { read_callback_ = cb; }
};