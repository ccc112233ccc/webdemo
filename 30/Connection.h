#pragma once
#include <atomic>
#include <functional>
#include <memory>

#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"

class Connection;
using spConnection = std::shared_ptr<Connection>;
class Connection : public std::enable_shared_from_this<Connection> {
 private:
  const std::unique_ptr<EventLoop>& loop_;
  std::unique_ptr<Socket> clientsock_;
  std::unique_ptr<Channel> clientch_;
  Buffer input_buffer_;
  Buffer output_buffer_;
  std::atomic<bool> disconnected_{false};

  std::function<void(spConnection)> close_callback_;
  std::function<void(spConnection)> error_callback_;
  std::function<void(spConnection, std::string&)> message_callback_;
  std::function<void(spConnection)> complete_callback_;

 public:
  Connection(const std::unique_ptr<EventLoop>& loop,
             std::unique_ptr<Socket> clientsock);
  ~Connection();
  int fd() const { return clientsock_->fd(); }
  std::string ip() const { return clientsock_->ip(); }
  uint16_t port() const { return clientsock_->port(); }
  void on_message();
  void close_callback();
  void error_callback();
  void write_callback();

  void set_close_callback(std::function<void(spConnection)> cb) {
    close_callback_ = cb;
  }
  void set_error_callback(std::function<void(spConnection)> cb) {
    error_callback_ = cb;
  }
  void set_message_callback(
      std::function<void(spConnection, std::string&)> cb) {
    message_callback_ = cb;
  }
  void set_complete_callback(std::function<void(spConnection)> cb) {
    complete_callback_ = cb;
  }

  void send(const char* data, size_t len);
};