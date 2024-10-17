#ifndef DISPLAY_H
#define DISPLAY_H

// https://github.com/olikraus/u8g2/wiki/u8x8reference
#include <U8x8lib.h>
#include <WString.h>

class Display {
  U8X8_SSD1306_128X64_NONAME_HW_I2C display;

 public:
  Display() : display(SCL, SDA, U8X8_PIN_NONE) {}

  void setup(bool flip = false) {
    display.begin();
    display.setFlipMode(flip);
    display.setFont(u8x8_font_chroma48medium8_r);
  }

  void drawString(int line, int indent, String s) { display.drawString(indent, line, s.c_str()); }

  void loopStep() {}
};

#endif

