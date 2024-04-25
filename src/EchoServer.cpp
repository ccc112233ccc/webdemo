#include "EchoServer.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <iostream>

#include "Tracer.h"

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
  // spdlog::info("new connection(fd = {}, ip = {}, port = {})", conn->fd(),
  //              conn->ip(), conn->port());
}

void EchoServer::HandleCloseConnection(spConnection conn) {
  // spdlog::info("close connection(fd = {}, ip = {}, port = {})", conn->fd(),
  //              conn->ip(), conn->port());
}

void EchoServer::HandleErrorConnection(spConnection conn) {
  spdlog::error("error connection(fd = {}, ip = {}, port = {})", conn->fd(),
                conn->ip(), conn->port());
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
  // spdlog::debug(
  //     "receive message from fd = {}, ip = {}, port = {}, message = {}",
  //     conn->fd(), conn->ip(), conn->port(), message);
  //   HTTP/1.0 200 OK
  // Content-Type: text/html
  // Content-Length: 25

  // <html><body>Hello!</body></html>
  std::string response =
      "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nConnection: "
      "keep-alive\r\nContent-Length: "
      "25\r\n\r\n<html><body>Hello!</body></html>";
  conn->send(response.data(), response.size());
}