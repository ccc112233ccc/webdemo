#pragma once
#include <iostream>
#include <string>

class Buffer {
 private:
  std::string buffer_;
  const uint16_t sep_;  // 0 无分隔符(固定长度，视频会议等)， 1， 四字节报头，
                        // 2， 分隔符'\r\n\r\n'

 public:
  Buffer(uint16_t sep = 0);
  ~Buffer();

  void append(const char* data, size_t len);
  void appendwithsep(const char* data, size_t len);
  void erase(size_t pos, size_t len);
  size_t size();
  const char* data();
  void clear();
  bool pickmessage(
      std::string& message);  //  从buffer中取出一条完整的消息， 返回true
                              // 说明取到了，否则返回false
};