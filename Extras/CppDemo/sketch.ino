#include "UsingDisplayLibrary.h"

Display display;

void setup() {
  std::cout << "setup" << std::endl;
  display.setup();
}

void loop() { display.loopStep(); }
