#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "Acceptor.h"
#include "Channel.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "ThreadPool.h"
class TcpServer {
 private:
  std::unique_ptr<EventLoop>
      mainloop_;  // 一个TcpServer对象对应一个主EventLoop对象
  std::vector<std::unique_ptr<EventLoop>>
      subloops_;   // 一个TcpServer对象对应多个子EventLoop对象
  int threadnum_;  // 线程数

  ThreadPool threadpoll_;  // 线程池
  Acceptor acceptor_;      // 一个TcpServer对象对应一个Acceptor对象

  std::mutex mutex_;
  std::map<int, spConnection> connections_;

  // 用于处理消息的回调函数
  std::function<void(spConnection)> new_connect_callback_;
  std::function<void(spConnection)> close_connect_callback_;
  std::function<void(spConnection)> error_connect_callback_;
  std::function<void(spConnection, std::string&)> message_callback_;
  std::function<void(spConnection)> complete_callback_;
  std::function<void(EventLoop*)> timeout_callback_;

 public:
  TcpServer(int port, int threadnum = 3);
  TcpServer(const std::string& ip, int port);
  ~TcpServer();
  void start();
  void stop();
  void new_connect(std::unique_ptr<Socket> clientsock);
  void close_connect(spConnection conn);
  void error_connect(spConnection conn);

  void message_callback(spConnection conn,
                        std::string& message);  // 用于处理消息的回调函数
  void complete_callback(spConnection conn);  // 用于处理消息的回调函数

  void epoll_timeout(EventLoop* loop);

  void set_new_connect_callback(std::function<void(spConnection)> cb) {
    new_connect_callback_ = cb;
  }
  void set_close_connect_callback(std::function<void(spConnection)> cb) {
    close_connect_callback_ = cb;
  }
  void set_error_connect_callback(std::function<void(spConnection)> cb) {
    error_connect_callback_ = cb;
  }
  void set_message_callback(
      std::function<void(spConnection, std::string&)> cb) {
    message_callback_ = cb;
  }
  void set_complete_callback(std::function<void(spConnection)> cb) {
    complete_callback_ = cb;
  }
  void set_timeout_callback(std::function<void(EventLoop*)> cb) {
    timeout_callback_ = cb;
  }

  void remove_connection(int fd) { connections_.erase(fd); }
};