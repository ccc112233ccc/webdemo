#include "EchoServer.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: %s <port>\n", argv[0]);
    return 1;
  }

  EchoServer echoserver(atoi(argv[1]), 1, 0);

  echoserver.start();
}