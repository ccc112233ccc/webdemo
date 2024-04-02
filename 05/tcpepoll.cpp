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
#include"Socket.h"
#include"Epoll.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    Socket servsock(createNonblockingSocket());
    InetAddress servaddr(atoi(argv[1]));
    servsock.setReuseAddr(true);
    servsock.setReusePort(true);
    servsock.setKeepAlive(true);
    servsock.setNoDelay(true);
    servsock.bindAddress(servaddr);
    servsock.listen(10);

    // // 创建epoll
    // int epfd = epoll_create(1);
    // if (epfd == -1) {
    //     perror("epoll_create");
    //     return 1;
    // }

    // // 添加监听套接字到epoll
    // epoll_event ev;
    // ev.events = EPOLLIN;
    // ev.data.fd = servsock.fd();

    // if (epoll_ctl(epfd, EPOLL_CTL_ADD, servsock.fd(), &ev) == -1) {
    //     perror("epoll_ctl");
    //     return 1;
    // }

    // // 创建epoll事件数组
    // epoll_event events[10];
    Epoll ep;
    ep.add_fd(servsock.fd(), EPOLLIN); // 添加监听套接字到epoll
    std::vector<epoll_event> events;

    while (true) {
        events = ep.wait();

        for (auto& ev : events) {
            if (ev.events & EPOLLRDHUP) {
                // 对端关闭连接
                printf("对端关闭连接\n");
                close(ev.data.fd);
                continue;
            } else if (ev.events && (EPOLLIN | EPOLLPRI)) {
                // 有数据可以读
                if (ev.data.fd == servsock.fd()) { // 有新连接
                    // sockaddr_in client_addr;
                    // socklen_t client_len = sizeof(client_addr);
                    InetAddress client_addr;
                    Socket* clientsock = new Socket(servsock.accept(client_addr));

                    // 接受连接，非阻塞
                    // int clientfd = accept4(listenfd, (struct sockaddr*)&client_addr, &client_len, SOCK_NONBLOCK);
                    // if (clientfd == -1) {
                    //     perror("accept");
                    //     return 1;
                    // }
                    // InetAddress client_addr_inet(client_addr);
                    printf("新连接：%s:%d\n", client_addr.ip(), client_addr.port());


                    // ev.events = EPOLLIN | EPOLLET; // 边缘触发
                    // ev.data.fd = clientsock->fd();
                    // if (epoll_ctl(epfd, EPOLL_CTL_ADD, clientsock->fd(), &ev) == -1) {
                    //     perror("epoll_ctl");
                    //     return 1;
                    // }
                    ep.add_fd(clientsock->fd(), EPOLLIN | EPOLLET); // 添加新连接到epoll
                } else { // 有数据可读
                    char buf[1024];
                    while(true) {
                        memset(buf, 0, sizeof(buf));
                        int n = recv(ev.data.fd, buf, sizeof(buf), 0);
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
                            close(ev.data.fd);
                            break;
                        } else {
                            printf("recv: %s\n", buf);
                        }
                    }
                }
            } else if (ev.events & EPOLLOUT) {
                // 有数据可以写
            } else {
                printf("未知事件\n");
            }
            
        }
    }


}