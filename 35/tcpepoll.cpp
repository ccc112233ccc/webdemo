#include <signal.h>

#include "EchoServer.h"

EchoServer* echoserver;

void sig_handler(int signo) {
  if (signo == SIGINT || signo == SIGTERM) {
    printf("receive SIGINT\n");
    delete echoserver;
    exit(0);
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: %s <port>\n", argv[0]);
    return 1;
  }

  signal(SIGINT, sig_handler);
  signal(SIGTERM, sig_handler);

  echoserver = new EchoServer(atoi(argv[1]), 3, 2);

  echoserver->start();
}