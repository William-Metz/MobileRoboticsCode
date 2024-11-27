#ifndef USING_DISPLAY_LIBRARY_H
#define USING_DISPLAY_LIBRARY_H

#include "DisplayLibrary.h"

class Display {
  DisplayDriver driver;

 public:
  Display(int a = 42) : driver(a) {
    std::cout << "Display default constructor" << std::endl;
    // driver = DisplayDriver(44);
  }

  void setup() {}
  void loopStep() {}
};

#endif
