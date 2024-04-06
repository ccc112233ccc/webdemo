#include "Acceptor.h"

Acceptor::Acceptor(const std::unique_ptr<EventLoop>& loop, int port)
    : loop_(loop),
      servsock_(createNonblockingSocket()),
      servch_(loop, servsock_.fd()) {
  InetAddress servaddr(port);
  servsock_.setReuseAddr(true);
  servsock_.setReusePort(true);
  servsock_.setKeepAlive(true);
  servsock_.setNoDelay(true);
  servsock_.bindAddress(servaddr);
  servsock_.listen(10);

  servch_.enable_reading();  // 启用读事件
  servch_.set_read_callback(std::bind(&Acceptor::new_connect, this));
}

Acceptor::~Acceptor() {
  // delete servsock_;
  // delete servch_;
}

#include "Connection.h"
void Acceptor::new_connect() {
  InetAddress client_addr;
  std::unique_ptr<Socket> clientsock(new Socket(servsock_.accept(client_addr)));

  clientsock->set_ip_port(client_addr);
  if (new_connection_callback_) {
    new_connection_callback_(std::move(clientsock));
  }
}