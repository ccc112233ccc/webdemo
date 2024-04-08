#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void alarm_handler(int sig) {
  printf("alarm_handler\n");
  alarm(3);  // 重新设置定时器
}

int main() {
  signal(SIGALRM, alarm_handler);
  alarm(3);
  while (1) {
    printf("main\n");
    sleep(1);
  }
  return 0;
}