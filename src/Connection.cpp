#include "Connection.h"

#include <spdlog/spdlog.h>

#include <bitset>

std::atomic<int> Connection::next_id_{0};

Connection::Connection(EventLoop* loop, std::unique_ptr<Socket> clientsock)
    : loop_(loop),
      clientsock_(std::move(clientsock)),
      clientch_(new Channel(loop_, clientsock_->fd())),
      id_(get_next_id()) {
  clientch_->set_read_callback(std::bind(&Connection::on_message, this));
  clientch_->set_close_callback(std::bind(&Connection::close_callback, this));
  clientch_->set_error_callback(std::bind(&Connection::error_callback, this));
  clientch_->set_write_callback(std::bind(&Connection::write_callback, this));
  clientch_->useet();
  // clientch_->enable_reading();
}

void Connection::enable_reading() { clientch_->enable_reading(); }

Connection::~Connection() { spdlog::debug("Connection {} destructed", id_); }

void Connection::close_callback() {
  if (disconnected_) {
    return;
  }
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
    spdlog::debug("send {} bytes", n);
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
    spdlog::debug("recv {} bytes", n);
    if (n == -1) {
      if (errno == EAGAIN) {
        std::string message;
        while (true) {
          if (input_buffer_.pickmessage(message) == false) {
            break;
          }

          lasttime_ = Timestamp::now();
          if (!message_callback_) {
            spdlog::warn("connection {} has no message callback", id_);
            break;
          }
          message_callback_(shared_from_this(), message);
        }

        break;
      } else {
        spdlog::error("recv error: {}", strerror(errno));
        return;
      }
    } else if (n == 0) {
      close_callback();
      break;
    } else {
      input_buffer_.append(buf, n);
    }
  }
}

void Connection::send(const char* data, size_t len) {
  if (disconnected_) {
    spdlog::warn("connection is disconnected, can't send data: {} bytes", len);
    return;
  }
  // if (loop_->in_loop_thread()) {
  if (true) {
    // 说明在 IO 线程中
    send_in_loop(data, len);
  } else {
    // 说明不在 IO 线程中， 将数据发送到 IO 线程中
    loop_->queue_in_loop(std::bind(&Connection::send_in_loop, this, data, len));
  }
}

void Connection::send_in_loop(const char* data, size_t len) {
  output_buffer_.appendwithsep(data, len);
  clientch_->enable_writing();
}