/*********
  Stephan Zehnder
  15.11.2022
  Controlling an Omega 3021 Flipflap-Display
*********/

#include <Webserver.h>
using namespace std;

Webserver webserver;

/**
 * Setup ESP-32 [Internal Function, do not change naming]
*/
void setup() {
  Serial.begin(115200);

  webserver = Webserver();

  // while(1); // Without this while-loop, thread creation (in Display::asyncPrint) fails
}

/**
 * Loop ESP-32 [Internal Function, do not change naming]
*/
void loop() {
  // int join_success = pthread_join(webserver.flapflap.threadHandle, NULL);
  // if (join_success != 0) {
  //     cout << "Couldn't join thread" << endl;
  // }

}
