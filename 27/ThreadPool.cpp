#include "ThreadPool.h"

void test() { std::cout << "test" << std::endl; }
void show(const std::string& str) { std::cout << str << std::endl; }
int main() {
  ThreadPool pool(4);
  pool.add_task(test);
  sleep(1);
  pool.add_task(std::bind(show, "hello"));
  sleep(1);
  pool.add_task(std::bind([]() { std::cout << "world" << std::endl; }));
  return 0;
}

// g++ -o ThreadPool ThreadPool.cpp