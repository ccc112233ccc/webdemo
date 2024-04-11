#include "Buffer.h"

Buffer::Buffer(uint16_t sep) : sep_(sep) {}
Buffer::~Buffer() {}

void Buffer::append(const char* data, size_t len) { buffer_.append(data, len); }

size_t Buffer::size() { return buffer_.size(); }

const char* Buffer::data() { return buffer_.data(); }

void Buffer::clear() { buffer_.clear(); }

void Buffer::erase(size_t pos, size_t len) { buffer_.erase(pos, len); }

void Buffer::appendwithsep(const char* data, size_t len) {
  if (sep_ == 0) {
    append(data, len);
  } else if (sep_ == 1) {
    uint32_t len_ = len;
    buffer_.append((char*)&len_, sizeof(len_));
    buffer_.append(data, len);
  } else if (sep_ == 2) {
    buffer_.append(data, len);
    buffer_.append("\r\n\r\n", 4);
  }
}

bool Buffer::pickmessage(std::string& message) {
  if (sep_ == 0) {
    if (buffer_.size() == 0) {
      return false;
    }
    message = buffer_;
    buffer_.clear();
    return true;
  } else if (sep_ == 1) {
    if (buffer_.size() < sizeof(uint32_t)) {
      return false;
    }
    uint32_t len = *(uint32_t*)buffer_.data();
    if (buffer_.size() < sizeof(uint32_t) + len) {
      return false;
    }
    message = std::string(buffer_.data() + sizeof(uint32_t), len);
    buffer_.erase(0, sizeof(uint32_t) + len);
    return true;
  } else if (sep_ == 2) {
    size_t pos = buffer_.find("\r\n\r\n");
    if (pos == std::string::npos) {
      return false;
    }
    message = buffer_.substr(0, pos);
    buffer_.erase(0, pos + 4);
    return true;
  }
  return false;
}