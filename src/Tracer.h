#pragma once
#include <cstdio>
#include <string>
class Tracer {
 public:
  Tracer(const char* file, const char* func, int line)
      : file_(file), func_(func), line_(line) {
    printf("Enter %s in %s at %d\n", func_, file_, line_);
  }
  ~Tracer() { printf("Exit %s in %s at %d\n", func_, file_, line_); }

 private:
  const char* file_;
  const char* func_;
  int line_;
};
#define TRACER_DEBUG

#ifdef TRACER_DEBUG
#define TRACE Tracer t(__FILE__, __func__, __LINE__);
#else
#define TRACE
#endif
