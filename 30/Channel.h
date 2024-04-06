#pragma once
#include <sys/epoll.h>

#include <functional>
#include <memory>

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
  std::function<void()> close_callback_;
  std::function<void()> error_callback_;
  std::function<void()> write_callback_;
  const std::unique_ptr<EventLoop>& loop_;

 public:
  Channel(const std::unique_ptr<EventLoop>& loop_, int fd);
  ~Channel();

  int fd();                // getter
  void useet();            // 边缘触发
  void enable_reading();   // 开启读事件
  void disable_reading();  // 关闭读事件
  void enable_writing();   // 开启写事件
  void disable_writing();  // 关闭写事件
  void disable_all();      // 关闭所有事件
  void remove();           // 从epoll中移除
  void setin_epoll();      // 设置在epoll中
  void set_revents(uint32_t revents);
  bool is_in_epoll();

  uint32_t events();   // getter
  uint32_t revents();  // getter

  void handle_events();
  void set_read_callback(std::function<void()> cb) { read_callback_ = cb; }
  void set_close_callback(std::function<void()> cb) { close_callback_ = cb; }
  void set_error_callback(std::function<void()> cb) { error_callback_ = cb; }
  void set_write_callback(std::function<void()> cb) { write_callback_ = cb; }
};