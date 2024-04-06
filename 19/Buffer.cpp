#include "Buffer.h"

Buffer::Buffer() {}
Buffer::~Buffer() {}

void Buffer::append(const char* data, size_t len) { buffer_.append(data, len); }

size_t Buffer::size() { return buffer_.size(); }

const char* Buffer::data() { return buffer_.data(); }

void Buffer::clear() { buffer_.clear(); }