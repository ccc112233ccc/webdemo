#include "Connection.h"

#include <bitset>

Connection::Connection(EventLoop* loop, std::unique_ptr<Socket> clientsock)
    : loop_(loop),
      clientsock_(std::move(clientsock)),
      clientch_(new Channel(loop_, clientsock_->fd())) {
  clientch_->set_read_callback(std::bind(&Connection::on_message, this));
  clientch_->set_close_callback(std::bind(&Connection::close_callback, this));
  clientch_->set_error_callback(std::bind(&Connection::error_callback, this));
  clientch_->set_write_callback(std::bind(&Connection::write_callback, this));
  clientch_->useet();
  clientch_->enable_reading();
}

Connection::~Connection() {
  // delete clientsock_;
  // delete clientch_;
  printf("Connection::~Connection()\n");
}

void Connection::close_callback() {
  disconnected_ = true;
  clientch_->remove();
  close_callback_(shared_from_this());
}

void Connection::error_callback() {
  disconnected_ = true;
  clientch_->remove();
  error_callback_(shared_from_this());
}
void Connection::write_callback() {
  if (output_buffer_.size() > 0) {
    int n = ::send(fd(), output_buffer_.data(), output_buffer_.size(), 0);
    if (n == -1) {
      perror("send");
    } else {
      output_buffer_.erase(0, n);
    }
  }
  if (output_buffer_.size() == 0) {
    clientch_->disable_writing();
    complete_callback_(shared_from_this());
  }
}

void Connection::on_message() {
  char buf[1024];
  while (true) {
    memset(buf, 0, sizeof(buf));
    int n = recv(fd(), buf, sizeof(buf), 0);
    if (n == -1) {
      if (errno == EAGAIN) {
        // printf("recv : %s\n", input_buffer_.data());
        while (true) {
          int len;
          memcpy(&len, input_buffer_.data(), sizeof(len));
          if (input_buffer_.size() < sizeof(len) + len) {
            // 说明数据还没接收完
            break;
          }
          std::string message(input_buffer_.data() + sizeof(len),
                              len);  // 从 input_buffer_ 中提取出一个完整的消息
          input_buffer_.erase(
              0, sizeof(len) + len);  // 从 input_buffer_ 中删除这个消息

          // message = "hello, " + message;
          // len = message.size();
          // std::string output_buffer(sizeof(len), 0);
          // memcpy(&output_buffer[0], &len, sizeof(len));
          // output_buffer.append(message);
          // send(fd(), output_buffer.data(), output_buffer.size(), 0);
          message_callback_(shared_from_this(), message);
        }

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

void Connection::send(const char* data, size_t len) {
  if (disconnected_) {
    printf("disconnected_\n");
    return;
  }
  output_buffer_.appendwithheader(data, len);
  clientch_->enable_writing();
}