#pragma once
#include<sys/epoll.h>

class Epoll;
class Socket;

class Channel {
private:
    int fd_;
    uint32_t events_;
    uint32_t revents_;
    Epoll* epoll_;
    bool inepoll_;
    bool is_listen_;

public:
    Channel(Epoll* epoll, int fd, bool is_listen);
    ~Channel();

    int fd(); // getter
    void useet(); // 边缘触发
    void enable_reading(); // 开启读事件
    void setin_epoll(); // 设置在epoll中
    void set_revents(uint32_t revents);
    bool is_in_epoll();

    uint32_t events(); // getter
    uint32_t revents(); // getter

    void handle_events(Socket& servsock);
};