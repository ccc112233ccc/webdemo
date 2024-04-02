#include"InetAddress.h"
/*
class InetAddress {
private:
    sockaddr_in addr_;
public:
    InetAddress(uint16_t port);
    InetAddress(const std::string &ip, uint16_t port);
    InetAddress(const sockaddr_in &addr): addr_(addr) {}
    ~InetAddress();

    const char* ip() const; // 返回点分十进制的ip地址
    uint16_t port() const; // 返回端口号
    const sockaddr* addr() const; // 返回sockaddr结构体
};
*/

InetAddress::InetAddress(uint16_t port) {
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = INADDR_ANY;
}

InetAddress::InetAddress(const std::string &ip, uint16_t port) {
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

InetAddress::~InetAddress() {}

const char* InetAddress::ip() const {
    return inet_ntoa(addr_.sin_addr);
}

uint16_t InetAddress::port() const {
    return ntohs(addr_.sin_port);
}

const sockaddr* InetAddress::addr() const {
    return (sockaddr*)&addr_;
}