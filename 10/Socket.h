#pragma once
#include <errno.h>
#include <netinet/tcp.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "InetAddress.h"

int createNonblockingSocket();  // 创建一个非阻塞的套接字

class Socket {
 private:
  const int fd_;

 public:
  Socket(int fd) : fd_(fd) {}
  ~Socket();
  int fd() const { return fd_; }

  void setReuseAddr(bool on);  // 设置地址复用
  void setReusePort(bool on);  // 设置端口复用
  void setKeepAlive(bool on);  // 设置keepalive
  void setNoDelay(bool on);    // 设置Nagle算法

  void bindAddress(const InetAddress &addr);  // 绑定地址
  void listen(int n = 128);                   // 监听
  int accept(InetAddress &peeraddr);          // 接受连接
};