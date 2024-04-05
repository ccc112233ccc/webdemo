#include "Socket.h"

// 创建一个非阻塞的套接字
int createNonblockingSocket() {
  int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  if (fd == -1) {
    perror("socket");
    exit(1);
  }
  return fd;
}

Socket::~Socket() { close(fd_); }

void Socket::setReuseAddr(bool on) {
  int opt = on ? 1 : 0;
  setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

void Socket::setReusePort(bool on) {
  int opt = on ? 1 : 0;
  setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
}

void Socket::setKeepAlive(bool on) {
  int opt = on ? 1 : 0;
  setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
}

void Socket::setNoDelay(bool on) {
  int opt = on ? 1 : 0;
  setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
}

void Socket::bindAddress(const InetAddress &addr) {
  if (::bind(fd_, addr.addr(), sizeof(sockaddr)) == -1) {
    perror("bind");
    exit(1);
  }
  ip_ = addr.ip();
  port_ = addr.port();
}

void Socket::listen(int n) {
  if (::listen(fd_, n) == -1) {
    perror("listen");
    exit(1);
  }
}

int Socket::accept(InetAddress &clientaddr) {
  sockaddr_in addr;
  socklen_t len = sizeof(sockaddr);
  int connfd = ::accept4(fd_, (sockaddr *)&addr, &len, SOCK_NONBLOCK);
  if (connfd == -1) {
    perror("accept");
    exit(1);
  }
  clientaddr.setAddr(addr);
  ip_ = clientaddr.ip();
  port_ = clientaddr.port();
  return connfd;
}