/*********
  Stephan Zehnder
  15.11.2022
  Controlling an Omega 3021 Flipflap-Display
*********/

#include <Arduino.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <Webserver.h>
using namespace std;





/**
 * Setup ESP-32 [Internal Function, do not change naming]
*/
void setup() {
  Serial.begin(115200);

  Webserver webserver;

  // TODO: Replace by "return 0;" or so?
  while(1);
}

/**
 * Loop ESP-32 [Internal Function, do not change naming]
*/
void loop() {
}
