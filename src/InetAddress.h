#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>

#include <string>

class InetAddress {
 private:
  sockaddr_in addr_;

 public:
  InetAddress() {}
  InetAddress(uint16_t port);
  InetAddress(const std::string &ip, uint16_t port);
  InetAddress(const sockaddr_in &addr) : addr_(addr) {}
  ~InetAddress();

  const char *ip() const;        // 返回点分十进制的ip地址
  uint16_t port() const;         // 返回端口号
  const sockaddr *addr() const;  // 返回sockaddr结构体
  void setAddr(const sockaddr_in &addr) { addr_ = addr; }
};