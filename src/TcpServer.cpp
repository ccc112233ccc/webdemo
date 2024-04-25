#include "TcpServer.h"

#include <functional>
#include <iostream>

#include "InetAddress.h"
#include "Tracer.h"

TcpServer::TcpServer(int port, int threadnum)
    : threadnum_(threadnum),
      mainloop_(new EventLoop(true)),
      acceptor_(mainloop_.get(), port),
      threadpoll_(threadnum, "IO") {
  acceptor_.set_new_connection_callback(
      std::bind(&TcpServer::new_connect, this, std::placeholders::_1));
  mainloop_->set_timeout_callback(
      std::bind(&TcpServer::epoll_timeout, this, std::placeholders::_1));

  for (int i = 0; i < threadnum_; i++) {
    subloops_.emplace_back(new EventLoop(false, 10, 30));
    subloops_[i]->set_timeout_callback(
        std::bind(&TcpServer::epoll_timeout, this, std::placeholders::_1));
    subloops_[i]->set_connection_timeout_cb(
        std::bind(&TcpServer::remove_connection, this, std::placeholders::_1));
    threadpoll_.add_task(std::bind(&EventLoop::loop, subloops_[i].get()));
  }
}

void TcpServer::start() { mainloop_->loop(); }
void TcpServer::stop() {
  mainloop_->stop();
  for (int i = 0; i < threadnum_; i++) {
    subloops_[i]->stop();
  }

  threadpoll_.stop();
}

TcpServer::~TcpServer() {}

void TcpServer::new_connect(std::unique_ptr<Socket> clientsock) {
  spConnection conn(new Connection(
      subloops_[clientsock->fd() % threadnum_].get(), std::move(clientsock)));
  conn->set_close_callback(
      std::bind(&TcpServer::close_connect, this, std::placeholders::_1));
  conn->set_error_callback(
      std::bind(&TcpServer::error_connect, this, std::placeholders::_1));
  conn->set_message_callback(std::bind(&TcpServer::message_callback, this,
                                       std::placeholders::_1,
                                       std::placeholders::_2));
  conn->set_complete_callback(
      std::bind(&TcpServer::complete_callback, this, std::placeholders::_1));

  {
    std::lock_guard<std::mutex> lock(mutex_);
    connections_[conn->fd()] = conn;
  }
  subloops_[conn->fd() % threadnum_]->add_connection(conn);

  if (new_connect_callback_) {
    new_connect_callback_(conn);
  }
}

void TcpServer::close_connect(spConnection conn) {
  if (close_connect_callback_) {
    close_connect_callback_(conn);
  }
  {
    std::lock_guard<std::mutex> lock(mutex_);
    connections_.erase(conn->fd());
  }
  subloops_[conn->fd() % threadnum_]->delete_connection(conn);
}

void TcpServer::error_connect(spConnection conn) {
  if (error_connect_callback_) {
    error_connect_callback_(conn);
  }
  {
    std::lock_guard<std::mutex> lock(mutex_);
    connections_.erase(conn->fd());
  }
}

void TcpServer::message_callback(spConnection conn, std::string& message) {
  if (message_callback_) {
    message_callback_(conn, message);
  }
}

void TcpServer::complete_callback(spConnection conn) {
  if (complete_callback_) {
    complete_callback_(conn);
  }
}

void TcpServer::epoll_timeout(EventLoop* loop) {
  if (timeout_callback_) {
    timeout_callback_(mainloop_.get());
  }
}