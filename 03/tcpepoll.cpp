#include<stdio.h>
#include<stdlib.h>

#include<sys/epoll.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<netinet/tcp.h>

#include"InetAddress.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    // 创建监听套接字
    int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); // 非阻塞
    if (listenfd == -1) {
        perror("socket");
        return 1;
    }

    // 设置地址复用
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // 设置Nagle算法
    setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
    // 设置端口复用
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    // 设置keepalive
    setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));


    // 绑定地址
    // struct sockaddr_in addr;
    // addr.sin_family = AF_INET;
    // addr.sin_port = htons(atoi(argv[1]));
    // addr.sin_addr.s_addr = INADDR_ANY;
    InetAddress addr(atoi(argv[1]));

    if (bind(listenfd, addr.addr(), sizeof(sockaddr)) == -1) {
        perror("bind");
        return 1;
    }

    // 监听
    if (listen(listenfd, 10) == -1) { // 最大连接数为10，超过10个连接会返回ECONNREFUSED
        perror("listen");
        return 1;
    }

    // 创建epoll
    int epfd = epoll_create(1);
    if (epfd == -1) {
        perror("epoll_create");
        return 1;
    }

    // 添加监听套接字到epoll
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
        perror("epoll_ctl");
        return 1;
    }

    // 创建epoll事件数组
    epoll_event events[10];

    while (true) {
        int nfds = epoll_wait(epfd, events, 10, -1); // -1表示阻塞
        if (nfds == -1) {
            perror("epoll_wait");
            return 1;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].events & EPOLLRDHUP) {
                // 对端关闭连接
                printf("对端关闭连接\n");
                epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                close(events[i].data.fd);
                continue;
            } else if (events[i].events && (EPOLLIN | EPOLLPRI)) {
                // 有数据可以读
                if (events[i].data.fd == listenfd) { // 有新连接
                    sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);

                    // 接受连接，非阻塞
                    int clientfd = accept4(listenfd, (struct sockaddr*)&client_addr, &client_len, SOCK_NONBLOCK);
                    if (clientfd == -1) {
                        perror("accept");
                        return 1;
                    }
                    InetAddress client_addr_inet(client_addr);
                    printf("新连接：%s:%d\n", client_addr_inet.ip(), client_addr_inet.port());


                    ev.events = EPOLLIN | EPOLLET; // 边缘触发
                    ev.data.fd = clientfd;
                    if (epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev) == -1) {
                        perror("epoll_ctl");
                        return 1;
                    }
                } else { // 有数据可读
                    char buf[1024];
                    while(true) {
                        memset(buf, 0, sizeof(buf));
                        int n = recv(events[i].data.fd, buf, sizeof(buf), 0);
                        if (n == -1) {
                            if (errno == EAGAIN) {
                                break;
                            } else {
                                perror("recv");
                                return 1;
                            }
                        } else if (n == 0) {
                            // 对端关闭连接
                            printf("对端关闭连接（n = 0）\n");
                            epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                            close(events[i].data.fd);
                            break;
                        } else {
                            printf("recv: %s\n", buf);
                        }
                    }
                }
            } else if (events[i].events & EPOLLOUT) {
                // 有数据可以写
            } else {
                printf("未知事件\n");
            }
            
        }
    }


}