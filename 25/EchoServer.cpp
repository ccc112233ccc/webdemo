#include "EchoServer.h"

#include <iostream>

EchoServer::EchoServer(uint16_t port) : server_(port) {
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

EchoServer::~EchoServer() {}

void EchoServer::start() { server_.start(); }

void EchoServer::HandleNewConnection(Connection* conn) {
  std::cout << "New connection" << std::endl;
}

void EchoServer::HandleCloseConnection(Connection* conn) {
  std::cout << "Close connection" << std::endl;
}

void EchoServer::HandleErrorConnection(Connection* conn) {
  std::cout << "Error connection" << std::endl;
}

void EchoServer::HandleMessage(Connection* conn, std::string& message) {
  std::cout << "Message: " << message << std::endl;
  message = "reply: " + message;
  // int len = message.size();
  // std::string output_buffer((char*)&len, sizeof(len));
  // output_buffer.append(message);
  // // send(conn->fd(), output_buffer.data(), output_buffer.size(), 0);
  conn->send(message.data(), message.size());
}

void EchoServer::HandleComplete(Connection* conn) {
  std::cout << "Complete" << std::endl;
}

void EchoServer::HandleTimeout(EventLoop* loop) {
  std::cout << "Timeout" << std::endl;
}