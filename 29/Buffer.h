#pragma once
#include <iostream>
#include <string>

class Buffer {
 private:
  std::string buffer_;

 public:
  Buffer();
  ~Buffer();

  void append(const char* data, size_t len);
  void appendwithheader(const char* data, size_t len);
  void erase(size_t pos, size_t len);
  size_t size();
  const char* data();
  void clear();
};