/*********
  Stephan Zehnder
  15.11.2022
  Controlling an Omega 3021 Flipflap-Display
*********/

#include <Arduino.h>
#include <string>
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <webserver.hpp>
using namespace std;

// Define Screen Size
int numOfRows = 3;
int numOfCols = 13;

// Define IO-Pins
int START = 22;
int ADL[] = {25, 26, 27, 4};
int ADC[] = {21, 19, 18, 17, 16};
int DATA[] = {32, 33, 34, 35, 36, 39};
char supportedCharacters[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','V','O','P','Q','R','S','T','U','V','W','X','Y','Z','/','-','1','2','3','4','5','6','7','8','9','0','.',' '};

// Select (HIGH) or deselect (LOW) each ADL-Pin (i.e. each row)
// l: 0, 1, 2 or 3
// val: HIGH or LOW
void setADL(int l, uint8_t val) {
  digitalWrite(ADL[l], val);
}

// c: Column-number ([0-n]), n_max=31
//    c=0: Modul on the very left (--> ADC=01111 (ADC[3]-ADC[0] are inverted))
//    c=31: Modul on the very right (--> ADC=10000)
void selectADC(int c) {
  digitalWrite(ADC[4], (c/16 == 1));  // ADC[4] refers to the EN pin of the Multiplexer-chip, thus it is inverted
  c = c%16;
  digitalWrite(ADC[3], (c/8 == 0));
  c = c%8;
  digitalWrite(ADC[2], (c/4 == 0));
  c = c%4;
  digitalWrite(ADC[1], (c/2 == 0));
  c = c%2;
  digitalWrite(ADC[0], (c/1 == 0));
}

/**
 * Setup ESP-32
 * [Internal Function, do not change naming]
*/
void setup() {
  Serial.begin(115200);
  setupWebserver();

  pinMode(START, OUTPUT);
  for (int i=0; i<=3; i++) {
    pinMode(ADL[i], OUTPUT);
  }
  for (int i=0; i<=4; i++) {
    pinMode(ADC[i], OUTPUT);
  }
  for (int i=0; i<=5; i++) {
    pinMode(DATA[i], INPUT);
  }
}

/**
 * @returns String containing binary code of currently selected module
*/
string getCurrentBinaryCode() {
  string binaryCode = "";
  for (int i=5; i>=0; i--) {
    binaryCode.append(to_string(!digitalRead(DATA[i])));
  }
  return binaryCode;
}

/**
 * @returns Character of currently selected module
*/
char getCurrentChar() {
  string binaryCode = getCurrentBinaryCode();
  if (binaryCode == "000000") {
    return '+'; // just return some non-existent value
  } 
  int intCode = (binaryCode[5]-'0')*1 + (binaryCode[4]-'0')*2 + (binaryCode[3]-'0')*4 + (binaryCode[2]-'0')*8 + (binaryCode[1]-'0')*16 + (binaryCode[0]-'0')*32;

  // intCode 1-26: Characters (A-Z)
  // intCode 45-57: Numbers (0-9) and dash (-)
  // intCode 32, 39: Space ( ) and slash (/)
  // Else: return some non-existent value (+)
  if (intCode >= 1 && intCode <= 26) {
    return char(64 + intCode);
  } else if (intCode >= 45 && intCode <= 57) {
    return char(intCode);
  } else if (intCode == 32) {
    return ' ';
  } else if (intCode == 39) {
    return '/';
  }
  return '+'; 
}

string reviseText(string text) {
  for (int j=0; j<size(text); j++) {
    text[j] = toupper(text[j]);
    if (find(supportedCharacters, supportedCharacters+size(supportedCharacters), text[j]) == (supportedCharacters+size(supportedCharacters))) {
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

void loop() {
  string text[] = {"", "", ""};
  int isCorrect[numOfRows * numOfCols];
  for (int i=0; i<numOfRows * numOfCols; i++) {
    isCorrect[i] = 10;
  }

  // Iterate through the Matrix (Row-wise)
  while(1) {
    if (isDraftChange(numOfRows-1)) {
      text[0] = reviseText(getDraft(0));
      text[1] = reviseText(getDraft(1));
      text[2] = reviseText(getDraft(2));
      for (int i=0; i<numOfRows * numOfCols; i++) {
        isCorrect[i] = 0;
      }
    }

    for (int i=0; i<numOfRows; i++) {
      string line = text[i];
      for (int j=0; j<numOfCols; j++) {
        if (isCorrect[numOfCols*i + j] < 10) {
          char myChar = line[j];

          // Try to stop the motor
          digitalWrite(START, LOW);
          usleep(5);
          selectADC(j);
          setADL(i, HIGH);

          // Stop if myChar is found
          char currentChar = getCurrentChar();
          if(currentChar == myChar) {
            digitalWrite(START, LOW);
            isCorrect[numOfCols*i + j]++;
          } else {
            digitalWrite(START, HIGH);
            isCorrect[numOfCols*i + j] = 0;
          }

          // Un-select the module, so it continues turning (if character was not found)
          usleep(5);
          setADL(i, LOW);
          selectADC(31);
        }
      }
    } 
  }
}

//   chrono::steady_clock::time_point begin = chrono::steady_clock::now();
//   chrono::steady_clock::time_point end = chrono::steady_clock::now();
//   cout << endl << "  Waited for valid position for " << chrono::duration_cast<chrono::microseconds>(end - begin).count() << "[µs]" << endl;
