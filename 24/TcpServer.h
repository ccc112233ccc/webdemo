#pragma once

#include <map>
#include <string>

#include "Acceptor.h"
#include "Channel.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"
class TcpServer {
 private:
  EventLoop loop_;  // 一个TcpServer对象对应多个EventLoop对象，现在只有一个
  Acceptor* acceptor_;  // 一个TcpServer对象对应一个Acceptor对象
  std::map<int, Connection*> connections_;

  // 用于处理消息的回调函数
  std::function<void(Connection*)> new_connect_callback_;
  std::function<void(Connection*)> close_connect_callback_;
  std::function<void(Connection*)> error_connect_callback_;
  std::function<void(Connection*, std::string&)> message_callback_;
  std::function<void(Connection*)> complete_callback_;
  std::function<void(EventLoop*)> timeout_callback_;

 public:
  TcpServer(int port);
  TcpServer(const std::string& ip, int port);
  ~TcpServer();
  void start();
  void new_connect(Socket* clientsock);
  void close_connect(Connection* conn);
  void error_connect(Connection* conn);

  void message_callback(Connection* conn,
                        std::string& message);  // 用于处理消息的回调函数
  void complete_callback(Connection* conn);  // 用于处理消息的回调函数

  void epoll_timeout(EventLoop* loop);

  void set_new_connect_callback(std::function<void(Connection*)> cb) {
    new_connect_callback_ = cb;
  }
  void set_close_connect_callback(std::function<void(Connection*)> cb) {
    close_connect_callback_ = cb;
  }
  void set_error_connect_callback(std::function<void(Connection*)> cb) {
    error_connect_callback_ = cb;
  }
  void set_message_callback(std::function<void(Connection*, std::string&)> cb) {
    message_callback_ = cb;
  }
  void set_complete_callback(std::function<void(Connection*)> cb) {
    complete_callback_ = cb;
  }
  void set_timeout_callback(std::function<void(EventLoop*)> cb) {
    timeout_callback_ = cb;
  }
};