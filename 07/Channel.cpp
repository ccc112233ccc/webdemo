#include "Channel.h"

#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"

Channel::Channel(Epoll* epoll, int fd, bool is_listen)
    : epoll_(epoll),
      fd_(fd),
      events_(0),
      revents_(0),
      inepoll_(false),
      is_listen_(is_listen) {}

Channel::~Channel() {}

int Channel::fd() { return fd_; }

void Channel::useet() { events_ |= EPOLLET; }

void Channel::enable_reading() {
  events_ |= EPOLLIN;
  epoll_->update_channel(this);
}

void Channel::setin_epoll() { inepoll_ = true; }

void Channel::set_revents(uint32_t revents) { revents_ = revents; }

bool Channel::is_in_epoll() { return inepoll_; }

uint32_t Channel::events() { return events_; }

uint32_t Channel::revents() { return revents_; }

void Channel::handle_events(Socket& servsock) {
  if (revents_ & EPOLLRDHUP) {
    // 对端关闭连接
    printf("对端关闭连接\n");
    close(fd_);
  } else if (revents_ && (EPOLLIN | EPOLLPRI)) {
    // 有数据可以读
    if (is_listen_) {  // 有新连接
      InetAddress client_addr;
      Socket* clientsock = new Socket(servsock.accept(client_addr));

      printf("新连接：%s:%d\n", client_addr.ip(), client_addr.port());

      Channel* clientch = new Channel(epoll_, clientsock->fd(), false);
      clientch->useet();           // 使用边缘触发
      clientch->enable_reading();  // 启用读事件
      // ep.add_fd(clientsock->fd(), EPOLLIN | EPOLLET); // 添加新连接到epoll
    } else {  // 有数据可读
      char buf[1024];
      while (true) {
        memset(buf, 0, sizeof(buf));
        int n = recv(fd_, buf, sizeof(buf), 0);
        if (n == -1) {
          if (errno == EAGAIN) {
            break;
          } else {
            perror("recv");
            return;
          }
        } else if (n == 0) {
          // 对端关闭连接
          printf("对端关闭连接（n = 0）\n");
          close(fd_);
          break;
        } else {
          printf("recv: %s\n", buf);
        }
      }
    }
  } else if (revents_ & EPOLLOUT) {
    // 有数据可以写
  } else {
    printf("未知事件\n");
  }
}
