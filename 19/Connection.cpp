#include "Connection.h"

Connection::Connection(EventLoop* loop, Socket* clientsock)
    : loop_(loop), clientsock_(clientsock) {
  clientch_ = new Channel(loop_, clientsock_->fd());
  clientch_->set_read_callback(std::bind(&Connection::on_message, this));
  clientch_->set_close_callback(std::bind(&Connection::close_callback, this));
  clientch_->set_error_callback(std::bind(&Connection::error_callback, this));
  clientch_->useet();
  clientch_->enable_reading();
}

Connection::~Connection() {
  delete clientsock_;
  delete clientch_;
}

void Connection::close_callback() { close_callback_(this); }

void Connection::error_callback() { error_callback_(this); }

void Connection::on_message() {
  char buf[1024];
  while (true) {
    memset(buf, 0, sizeof(buf));
    int n = recv(fd(), buf, sizeof(buf), 0);
    if (n == -1) {
      if (errno == EAGAIN) {
        printf("recv : %s\n", input_buffer_.data());

        input_buffer_.clear();
        break;
      } else {
        perror("recv");
        return;
      }
    } else if (n == 0) {
      // 对端关闭连接
      // printf("对端关闭连接（n = 0）\n");
      // close(fd_);
      close_callback();
      break;
    } else {
      input_buffer_.append(buf, n);

      // 返回一个有效的 HTTP 响应
      // const char* response =
      //     "HTTP/1.1 200 OK\r\n"
      //     "Content-Type: text/plain\r\n"
      //     "Content-Length: 12\r\n"
      //     "\r\n"
      //     "Hello, world";
      // send(fd_, response, strlen(response), 0);
    }
  }
}