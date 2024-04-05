#include "TcpServer.h"

#include <functional>

#include "InetAddress.h"

TcpServer::TcpServer(int port) {
  acceptor_ = new Acceptor(&loop_, port);
  acceptor_->set_new_connection_callback(
      std::bind(&TcpServer::new_connect, this, std::placeholders::_1));
}

void TcpServer::start() { loop_.loop(); }

TcpServer::~TcpServer() {}

void TcpServer::new_connect(Socket* clientsock) {
  Connection* conn = new Connection(&loop_, clientsock);
}