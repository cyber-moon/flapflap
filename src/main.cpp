/*********
  Stephan Zehnder
  15.11.2022
  Controlling an Omega 3021 Flipflap-Display
*********/

#include <Arduino.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <webserver.h>
#include <display.h>
using namespace std;

// Define Screen Size
int numOfRows = 3;
int numOfCols = 13;

char supportedCharacters[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','V','O','P','Q','R','S','T','U','V','W','X','Y','Z','/','-','1','2','3','4','5','6','7','8','9','0','.',' '};


/**
 * Remove unsupported Characters and, if necessary, add spaces at the end of line
 * @param text  single row of text, possibly containing unsupported characters (e.g. "fräch!")
 * @returns     single row of text, with characters replaced an missing spaces (e.g. "fr/ch/     ")
*/
string reviseText(string text) {
  char* supportedCharacters_end = supportedCharacters+size(supportedCharacters);
  for (int j=0; j<size(text); j++) {
    text[j] = toupper(text[j]);
    if (find(supportedCharacters, supportedCharacters_end, text[j]) == supportedCharacters_end) {
      cout << "Unsupported character: " << text[j] << endl;
      text[j] = '/';
    }
  }
  if (size(text) < numOfCols) {
    int missingSpaces = numOfCols - size(text);
    char spaces[missingSpaces];
    fill_n(spaces, missingSpaces, ' ');
    text = text.append(spaces);
  }
  return text;
}

/**
 * Setup ESP-32 [Internal Function, do not change naming]
*/
void setup() {
  Serial.begin(115200);
  setupWebserver();

  Display flapflap;


  string text[] = {"", "", ""};
  int isCorrect[numOfRows * numOfCols];
  fill_n(isCorrect, numOfRows * numOfCols, 10);

  // Iterate through the Matrix (Row-wise)
  while(1) {
    if (isDraftChange(numOfRows-1)) {
      text[0] = reviseText(getDraft(0));
      text[1] = reviseText(getDraft(1));
      text[2] = reviseText(getDraft(2));
      fill_n(isCorrect, numOfRows * numOfCols, 0);
    }

    for (int i=0; i<numOfRows; i++) {
      string line = text[i];
      for (int j=0; j<numOfCols; j++) {
        // Correct char needs to be recognized in 10 iterations in a row to be valid
        if (isCorrect[numOfCols*i + j] < 10) {
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
            isCorrect[numOfCols*i + j]++;
          } else {
          flapflap.setSTART(HIGH);
            isCorrect[numOfCols*i + j] = 0;
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
