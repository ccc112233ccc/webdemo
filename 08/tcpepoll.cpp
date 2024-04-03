#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <functional>

#include "Channel.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"

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
  servch.enable_reading();  // 启用读事件
  servch.set_read_callback(
      std::bind(&Channel::new_connect, &servch, std::ref(servsock)));

  std::vector<Channel*> channels;

  while (true) {
    channels = ep.wait();

    for (auto& ch : channels) {
      ch->handle_events();
    }
  }
}