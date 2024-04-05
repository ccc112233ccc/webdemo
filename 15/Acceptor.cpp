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
  servch_->set_read_callback(std::bind(&Acceptor::new_connect, this));
}

Acceptor::~Acceptor() {
  delete servsock_;
  delete servch_;
}

#include "Connection.h"
void Acceptor::new_connect() {
  InetAddress client_addr;
  Socket* clientsock = new Socket(servsock_->accept(client_addr));

  printf("新连接：%s:%d\n", client_addr.ip(), client_addr.port());

  new_connection_callback_(clientsock);
}