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

#include <iostream>

int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("Usage: %s <ip> <port>\n", argv[0]);
    return 1;
  }

  // 创建套接字
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("socket");
    return 1;
  }

  // 设置地址
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(atoi(argv[2]));
  addr.sin_addr.s_addr = inet_addr(argv[1]);

  // 连接
  if (connect(sockfd, (sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("connect");
    return 1;
  }
  // 开始时间
  printf("start %ld\n", time(NULL));
  char buf[1024];
  for (int i = 0; i < 1000000; i++) {
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "hello %d", i);

    char temp[1024];
    memset(temp, 0, sizeof(temp));
    int len = strlen(buf);
    memcpy(temp, &len, sizeof(len));
    memcpy(temp + sizeof(len), buf, len);
    // printf("请输入：");
    // scanf("%s", buf);
    send(sockfd, temp, sizeof(len) + len, 0);

    // 先读取报文头部
    recv(sockfd, &len, sizeof(len), 0);
    // 再读取报文体
    bzero(buf, sizeof(buf));
    recv(sockfd, buf, len, 0);
    // printf("recv: %s\n", buf);
  }
  // 结束时间
  printf("end   %ld\n", time(NULL));
}