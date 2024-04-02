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
#include"Channel.h"

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

    Epoll ep;
    // ep.add_fd(servsock.fd(), EPOLLIN); // 添加监听套接字到epoll
    Channel servch(&ep, servsock.fd());
    servch.enable_reading(); // 启用读事件
    std::vector<Channel*> channels;

    while (true) {
        channels = ep.wait();

        for (auto& ch : channels) {
            if (ch->events() & EPOLLRDHUP) {
                // 对端关闭连接
                printf("对端关闭连接\n");
                close(ch->fd());
                continue;
            } else if (ch->events() && (EPOLLIN | EPOLLPRI)) {
                // 有数据可以读
                if (ch->fd() == servsock.fd()) { // 有新连接
                    InetAddress client_addr;
                    Socket* clientsock = new Socket(servsock.accept(client_addr));

                    printf("新连接：%s:%d\n", client_addr.ip(), client_addr.port());


                    Channel* clientch = new Channel(&ep, clientsock->fd());
                    clientch->useet(); // 使用边缘触发
                    clientch->enable_reading(); // 启用读事件
                    // ep.add_fd(clientsock->fd(), EPOLLIN | EPOLLET); // 添加新连接到epoll
                } else { // 有数据可读
                    char buf[1024];
                    while(true) {
                        memset(buf, 0, sizeof(buf));
                        int n = recv(ch->fd(), buf, sizeof(buf), 0);
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
                            close(ch->fd());
                            break;
                        } else {
                            printf("recv: %s\n", buf);
                        }
                    }
                }
            } else if (ch->revents() & EPOLLOUT) {
                // 有数据可以写
            } else {
                printf("未知事件\n");
            }
            
        }
    }


}