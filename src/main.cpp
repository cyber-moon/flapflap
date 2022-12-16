/*********
  Stephan Zehnder
  15.11.2022
  Controlling an Omega 3021 Flipflap-Display
*********/

#include <Arduino.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <Display.h>
#include <Webserver.h>
using namespace std;





/**
 * Setup ESP-32 [Internal Function, do not change naming]
*/
void setup() {
  Serial.begin(115200);

  Webserver webserver;

  Display flapflap;

  vector<string> temp = {"asdf", "qwer", "yxcv"};

  	string text[] = {"", "", ""};

	int isCorrect[flapflap.numOfRows * flapflap.numOfCols];
	fill_n(isCorrect, flapflap.numOfRows * flapflap.numOfCols, 10);

	// Iterate through the Matrix (Row-wise)
	while(1) {
		if (webserver.isDraftChange(flapflap.numOfRows-1)) {
			text[0] = flapflap.reviseText(webserver.getDraft(0));
			text[1] = flapflap.reviseText(webserver.getDraft(1));
			text[2] = flapflap.reviseText(webserver.getDraft(2));
			fill_n(isCorrect, flapflap.numOfRows * flapflap.numOfCols, 0);
		}

		for (int i=0; i<flapflap.numOfRows; i++) {
			string line = text[i];
			for (int j=0; j<flapflap.numOfCols; j++) {
				// Correct char needs to be recognized in 10 iterations in a row to be valid
				if (isCorrect[flapflap.numOfCols*i + j] < 10) {
				char myChar = line[j];

				// Try to stop the motor by de-activating START and selecting the module
				flapflap.setSTART(LOW);
				usleep(5);
				flapflap.selectADC(j);
				flapflap.setADL(i, HIGH);

				// Stop if myChar is found (set START to LOW)
				char currentChar = flapflap.getCurrentChar();
				if(currentChar == myChar) {
				flapflap.setSTART(LOW);
					isCorrect[flapflap.numOfCols*i + j]++;
				} else {
				flapflap.setSTART(HIGH);
					isCorrect[flapflap.numOfCols*i + j] = 0;
				}

				// Un-select the module, so it continues turning (if character was not found)
				usleep(5);
				flapflap.setADL(i, LOW);
				flapflap.selectADC(31);
				}
			}
		} 

	}


}

/**
 * Loop ESP-32 [Internal Function, do not change naming]
*/
void loop() {
}
