#ifndef DISPLAY_LIBRARY_H
#define DISPLAY_LIBRARY_H

#include <iostream>

class DisplayDriver {
  int a;

 public:
  DisplayDriver() {
    std::cout << "DisplayDriver default constructor" << std::endl;
    a = 10;
  }

  DisplayDriver(int initA) {
    std::cout << "DisplayDriver one-arg constructor" << std::endl;
    a = initA;
  }

  void begin() {}
  void loopStep() {}
};

#endif
