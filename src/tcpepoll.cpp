#include <signal.h>
#include <spdlog/spdlog.h>

#include "EchoServer.h"
#include "Tracer.h"
#include "spdlog/cfg/env.h"
EchoServer* echoserver;

void sig_handler(int signo) {
  if (signo == SIGINT || signo == SIGTERM) {
    printf("receive SIGINT\n");
    delete echoserver;
    exit(0);
  }
}

int main(int argc, char* argv[]) {
  int port = 8080;
  if (argc >= 2) {
    port = atoi(argv[1]);
  }
  int io_threads = 3;
  if (argc >= 3) {
    io_threads = atoi(argv[2]);
  }
  int work_thread = 0;
  if (argc >= 4) {
    work_thread = atoi(argv[3]);
  }
  spdlog::cfg::load_env_levels();
  signal(SIGINT, sig_handler);
  signal(SIGTERM, sig_handler);

  echoserver = new EchoServer(port, io_threads, work_thread);

  echoserver->start();
}