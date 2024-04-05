#include "Connection.h"

Connection::Connection(EventLoop* loop, Socket* clientsock)
    : loop_(loop), clientsock_(clientsock) {
  clientch_ = new Channel(loop_, clientsock_->fd());
  clientch_->set_read_callback(std::bind(&Channel::on_message, clientch_));
  clientch_->useet();
  clientch_->enable_reading();
}

Connection::~Connection() {
  delete clientsock_;
  delete clientch_;
}