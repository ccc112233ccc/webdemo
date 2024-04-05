#include "TcpServer.h"

#include <functional>

#include "InetAddress.h"

TcpServer::TcpServer(int port) {
  // Socket* servsock = new Socket(createNonblockingSocket());
  // InetAddress servaddr(port);
  // servsock->setReuseAddr(true);
  // servsock->setReusePort(true);
  // servsock->setKeepAlive(true);
  // servsock->setNoDelay(true);
  // servsock->bindAddress(servaddr);
  // servsock->listen(10);

  // Channel* servch = new Channel(loop_.get_epoll(), servsock->fd());
  // servch->enable_reading();  // 启用读事件
  // servch->set_read_callback(std::bind(&Channel::new_connect, servch,
  // servsock));
  acceptor_ = new Acceptor(&loop_, port);
}

void TcpServer::start() { loop_.loop(); }

TcpServer::~TcpServer() {}