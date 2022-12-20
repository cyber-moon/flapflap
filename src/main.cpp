/*********
  Stephan Zehnder
  15.11.2022
  Controlling an Omega 3021 Flipflap-Display
*********/

#include <Arduino.h>
#include <string>
#include <iostream>
#include <unistd.h>
// #include <Display.h>
#include <Webserver.h>
using namespace std;





/**
 * Setup ESP-32 [Internal Function, do not change naming]
*/
void setup() {
  Serial.begin(115200);

  // Display flapflap;
  Webserver webserver;


  // vector<string> temp = {"asdf", "qwer", "yxcv"};

	// flapflap.printText(temp, webserver);
	// flapflap.printText();
  while(1) {}

}

/**
 * Loop ESP-32 [Internal Function, do not change naming]
*/
void loop() {
}
