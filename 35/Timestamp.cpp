#include "Timestamp.h"

#include <signal.h>

Timestamp::Timestamp() : secsinceepoch_(time(0)) {}

Timestamp::Timestamp(time_t secsinceepoch) : secsinceepoch_(secsinceepoch) {}

Timestamp Timestamp::now() { return Timestamp(time(0)); }

time_t Timestamp::toint() const { return secsinceepoch_; }

std::string Timestamp::tostr() const {
  char buf[32];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&secsinceepoch_));
  return buf;
}

// int main() {
//   printf("%s\n", Timestamp::now().tostr().c_str());
//   printf("%ld\n", Timestamp::now().toint());
//   return 0;
// }