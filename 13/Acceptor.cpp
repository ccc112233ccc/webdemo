#include "Acceptor.h"

Acceptor::Acceptor(EventLoop* loop, int port) : loop_(loop) {
  servsock_ = new Socket(createNonblockingSocket());
  InetAddress servaddr(port);
  servsock_->setReuseAddr(true);
  servsock_->setReusePort(true);
  servsock_->setKeepAlive(true);
  servsock_->setNoDelay(true);
  servsock_->bindAddress(servaddr);
  servsock_->listen(10);

  servch_ = new Channel(loop_, servsock_->fd());
  servch_->enable_reading();  // 启用读事件
  servch_->set_read_callback(
      std::bind(&Channel::new_connect, servch_, servsock_));
}

Acceptor::~Acceptor() {
  delete servsock_;
  delete servch_;
}