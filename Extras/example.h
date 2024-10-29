#ifndef EXAMPLE_H
#define EXAMPLE_H

// Include libraries
// - Arduino libraries are included with angle brackets
// - Third-party libraries are included with angle brackets
// - Local libraries are included with quotes

class Example {
  // Private variables and methods
  // You can initialize plain-old-data (POD) inline
  int knownValue = 0;
  int userDefinedValue;
  LibraryObject libraryObject;

 public:
  // The constructor can initialize member variables in
  // 1. the member initializer list (the part between the colon and the opening brace), or
  // 2. the body of the constructor.
  // The member initializer list is preferred since it prevents accidentally calling a constructor
  //   twice when it is not necessary.
  Example(int userValue, float someArg) : userDefinedValue(userValue), libraryObject(someArg) {};

  // The setup method must be called in the Arduino setup function.
  // You should use the setup method to initialize complex objects,
  //   and anything that must be initialized AFTER the Arduino environment is ready.
  // You can also make change the method signature to suit your needs, for example,
  //   it can return a boolean to indicate success or failure or take an argument
  //   to enable or disable certain features.
  void setup() {}

  // The loopStep method must be called in the Arduino loop function.
  // You should use the loopStep method to perform any actions that must be repeated.
  // You can also make change the method signature to suit your needs, for example,
  //   it can return a boolean to indicate success or failure or take an argument
  //   to enable or disable certain features.
  void loopStep() {}

  // You can define additional public methods
};

#endif
