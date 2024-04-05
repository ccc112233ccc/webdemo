#include "Connection.h"

Connection::Connection(EventLoop* loop, Socket* clientsock)
    : loop_(loop), clientsock_(clientsock) {
  clientch_ = new Channel(loop_, clientsock_->fd());
  clientch_->set_read_callback(std::bind(&Channel::on_message, clientch_));
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