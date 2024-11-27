#include "FakeArduino.h"
#include "sketch.ino"

int main() {
  std::cout << "main" << std::endl;
  init();
  setup();

  // while (true) {
  loop();
  // }
}
