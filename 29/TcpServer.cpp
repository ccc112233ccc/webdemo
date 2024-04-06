#include "TcpServer.h"

#include <functional>
#include <iostream>

#include "InetAddress.h"

TcpServer::TcpServer(int port, int threadnum) : threadnum_(threadnum) {
  mainloop_ = new EventLoop();
  acceptor_ = new Acceptor(mainloop_, port);
  acceptor_->set_new_connection_callback(
      std::bind(&TcpServer::new_connect, this, std::placeholders::_1));
  mainloop_->set_timeout_callback(
      std::bind(&TcpServer::epoll_timeout, this, std::placeholders::_1));

  threadpoll_ = new ThreadPool(threadnum_, "IO");  // 创建线程池
  for (int i = 0; i < threadnum_; i++) {
    EventLoop* loop = new EventLoop();
    subloops_.push_back(loop);
    loop->set_timeout_callback(
        std::bind(&TcpServer::epoll_timeout, this, std::placeholders::_1));
    threadpoll_->add_task(std::bind(&EventLoop::loop, loop));
  }
}

void TcpServer::start() { mainloop_->loop(); }

TcpServer::~TcpServer() {
  delete acceptor_;
  // for (auto& conn : connections_) {
  //   delete conn.second;
  // }
  delete mainloop_;
  delete threadpoll_;
  for (auto& loop : subloops_) {
    delete loop;
  }
}

void TcpServer::new_connect(Socket* clientsock) {
  spConnection conn(
      new Connection(subloops_[clientsock->fd() % threadnum_], clientsock));
  conn->set_close_callback(
      std::bind(&TcpServer::close_connect, this, std::placeholders::_1));
  conn->set_error_callback(
      std::bind(&TcpServer::error_connect, this, std::placeholders::_1));
  conn->set_message_callback(std::bind(&TcpServer::message_callback, this,
                                       std::placeholders::_1,
                                       std::placeholders::_2));
  conn->set_complete_callback(
      std::bind(&TcpServer::complete_callback, this, std::placeholders::_1));

  connections_[conn->fd()] = conn;

  if (new_connect_callback_) {
    new_connect_callback_(conn);
  }
}

void TcpServer::close_connect(spConnection conn) {
  if (close_connect_callback_) {
    close_connect_callback_(conn);
  }
  connections_.erase(conn->fd());
  // delete conn;
}

void TcpServer::error_connect(spConnection conn) {
  if (error_connect_callback_) {
    error_connect_callback_(conn);
  }
  connections_.erase(conn->fd());
  // delete conn;
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
    timeout_callback_(mainloop_);
  }
}