#include <iostream>

class VerboseStream {
  bool active;

public:
  VerboseStream(bool __active) : active{__active} {};

  template <class T> VerboseStream &operator<<(T val) {
    if (active) {
      std::cout << val;
    }
    return *this;
  }

  VerboseStream &operator<<(std::ostream &(*f)(std::ostream &)) {
    if (active) {
      f(std::cout);
    }
    return *this;
  }

  VerboseStream &operator<<(std::ostream &(*f)(std::ios &)) {
    if (active) {
      f(std::cout);
    }
    return *this;
  }

  VerboseStream &operator<<(std::ostream &(*f)(std::ios_base &)) {
    if (active) {
      f(std::cout);
    }
    return *this;
  }
};