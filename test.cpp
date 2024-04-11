#include <iostream>
#include <memory>
#include <typeinfo>
int main() {
  std::shared_ptr<int> p1(new int(5));

  auto func = [p1](int a) { std::cout << p1.use_count() << std::endl; };

  using lamtype = decltype(func);

  std::cout << typeid(lamtype).name() << std::endl;

  return 0;
}