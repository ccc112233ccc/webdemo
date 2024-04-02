#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/epoll.h>
#include<sys/types.h>
#include<vector>
#include<string.h>

class Channel;
class Epoll {
private:
    static const int MAX_EVENTS_ = 100;
    int epoll_fd_;
    epoll_event events_[MAX_EVENTS_];

public:
    Epoll();
    ~Epoll();

    void add_fd(int fd, uint32_t op);
    void update_channel(Channel* channel);
    std::vector<Channel*> wait();
};