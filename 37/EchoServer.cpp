#include "EchoServer.h"

#include <iostream>

EchoServer::EchoServer(uint16_t port, int threadnum, int workerthreadnum)
    : server_(port, threadnum), threadpoll_(workerthreadnum, "workerthread") {
  server_.set_new_connect_callback(
      std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
  server_.set_close_connect_callback(std::bind(
      &EchoServer::HandleCloseConnection, this, std::placeholders::_1));
  server_.set_error_connect_callback(std::bind(
      &EchoServer::HandleErrorConnection, this, std::placeholders::_1));
  server_.set_message_callback(std::bind(&EchoServer::HandleMessage, this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
  server_.set_complete_callback(
      std::bind(&EchoServer::HandleComplete, this, std::placeholders::_1));
  server_.set_timeout_callback(
      std::bind(&EchoServer::HandleTimeout, this, std::placeholders::_1));
}

EchoServer::~EchoServer() { stop(); }

void EchoServer::start() { server_.start(); }
void EchoServer::stop() {
  threadpoll_.stop();
  server_.stop();
}

void EchoServer::HandleNewConnection(spConnection conn) {
  printf("%s new connection(fd = %d, ip = %s, port = %d)\n",
         Timestamp::now().tostr().c_str(), conn->fd(), conn->ip().c_str(),
         conn->port());
}

void EchoServer::HandleCloseConnection(spConnection conn) {
  printf("%s close connection(fd = %d, ip = %s, port = %d)\n",
         Timestamp::now().tostr().c_str(), conn->fd(), conn->ip().c_str(),
         conn->port());
}

void EchoServer::HandleErrorConnection(spConnection conn) {
  // std::cout << "Error connection" << std::endl;
}

void EchoServer::HandleMessage(spConnection conn, std::string& message) {
  if (threadpoll_.size() == 0) {
    OnMessage(conn, message);
    return;
  }
  threadpoll_.add_task(std::bind(&EchoServer::OnMessage, this, conn, message));
}

void EchoServer::HandleComplete(spConnection conn) {
  // std::cout << "Complete" << std::endl;
}

void EchoServer::HandleTimeout(EventLoop* loop) {
  // std::cout << "Timeout" << std::endl;
}

void EchoServer::OnMessage(spConnection conn, std::string& message) {
  // printf("%s receive message(fd = %d, ip = %s, port = %d): %s\n",
  //        Timestamp::now().tostr().c_str(), conn->fd(), conn->ip().c_str(),
  //        conn->port(), message.c_str());
  message = "reply: " + message;
  conn->send(message.data(), message.size());
}