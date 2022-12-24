/*********
  Stephan Zehnder
  15.11.2022
  Controlling an Omega 3021 Flipflap-Display
*********/

#include <Webserver.h>
using namespace std;


/**
 * Setup ESP-32 [Internal Function, do not change naming]
*/
void setup() {
  Serial.begin(115200);

  Webserver webserver;

  while(1); // Without this while-loop, thread creation (in Display::asyncPrint) fails
}

/**
 * Loop ESP-32 [Internal Function, do not change naming]
*/
void loop() {
}
