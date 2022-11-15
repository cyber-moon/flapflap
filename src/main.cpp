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
using namespace std;

#define START 22
#define ADC_A 21
#define ADC_B 19
#define ADC_C 18
#define ADC_D 17
#define ADC_E 16
#define DATA_0 32
#define DATA_1 33
#define DATA_2 34
#define DATA_3 35
#define DATA_4 36
#define DATA_5 39

int ADL[] = {25, 26, 27, 4};

// l: 1, 2, 3 or 4
// val: HIGH or LOW
void setADL(int l, uint8_t val) {
  digitalWrite(ADL[l-1], val);
}

// TODO: Implement correct ADC-logic
void setADC(int l, uint8_t val) {
  digitalWrite(ADL[1], val);
}

void selectAllModules() {
  setADL(1, HIGH);
  setADL(2, HIGH);
  setADL(3, HIGH);
  setADL(4, HIGH);
  digitalWrite(ADC_A, HIGH);
  digitalWrite(ADC_B, HIGH);
  digitalWrite(ADC_C, HIGH);
  digitalWrite(ADC_D, HIGH);
  digitalWrite(ADC_E, HIGH);
}

void deselectAllModules() {
  setADL(1, LOW);
  setADL(2, LOW);
  setADL(3, LOW);
  setADL(4, LOW);
  digitalWrite(ADC_A, LOW);
  digitalWrite(ADC_B, LOW);
  digitalWrite(ADC_C, LOW);
  digitalWrite(ADC_D, LOW);
  digitalWrite(ADC_E, LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(ADL[0], OUTPUT);
  pinMode(ADL[1], OUTPUT);
  pinMode(ADL[2], OUTPUT);
  pinMode(ADL[3], OUTPUT);
  pinMode(ADC_A, OUTPUT);
  pinMode(ADC_B, OUTPUT);
  pinMode(ADC_C, OUTPUT);
  pinMode(ADC_D, OUTPUT);
  pinMode(ADC_E, OUTPUT);
  pinMode(START, OUTPUT);
  pinMode(DATA_0, INPUT);
  pinMode(DATA_1, INPUT);
  pinMode(DATA_2, INPUT);
  pinMode(DATA_3, INPUT);
  pinMode(DATA_4, INPUT);
  pinMode(DATA_5, INPUT);

  // Start all modules and then de-select them
  selectAllModules();
  digitalWrite(START, HIGH);
  deselectAllModules();
}

string getCurrentPosition() {
  string binaryCode = "";
  binaryCode.append(to_string(!!!digitalRead(DATA_5)));
  binaryCode.append(to_string(!!!digitalRead(DATA_4)));
  binaryCode.append(to_string(!!!digitalRead(DATA_3)));
  binaryCode.append(to_string(!!!digitalRead(DATA_2)));
  binaryCode.append(to_string(!!!digitalRead(DATA_1)));
  binaryCode.append(to_string(!!!digitalRead(DATA_0)));
  return binaryCode;
}

char getCurrentChar() {
  string binaryCode = getCurrentPosition();
  int ascii = 64 + (binaryCode[5-0]-'0')*1 + (binaryCode[5-1]-'0')*2 + (binaryCode[5-2]-'0')*4 + (binaryCode[5-3]-'0')*8 + (binaryCode[5-4]-'0')*16 + (binaryCode[5-5]-'0')*32;
  return char(ascii);
}

void loop() {
  int numOfRows = 1;
  int numOfCols = 1;

  // Iterate through the Matrix (Row-wise)
  while(1) {
    for (int i=0; i<numOfRows; i++) {
      string text = "BELLO WORLD";
      for (int j=0; j<numOfCols; j++) {
        char myChar = text[j];

        // Try to stop the motor
        digitalWrite(START, LOW);
        usleep(5);
        setADC(2, HIGH);            // TODO: Use 'j+1' instead of '2' as soon as ADC-interface works correctly (ADL_2 is currently used instead of ADC_1)
        setADL(i+1, HIGH);

        // if a valid character was detected
        if(!(getCurrentPosition() == "000000")) {
          cout << endl << getCurrentChar() << ": ";
          
          if(getCurrentChar() == myChar) {
            digitalWrite(START, LOW);
            cout << " YESSS, FOUND!" << endl;
          } else {
            digitalWrite(START, HIGH);
            cout << " Nope, still searching..." << endl;
          }
        } else {
            digitalWrite(START, HIGH);
            cout << "nv"; // Not valid position
        }

        // Un-select the module, so it continues turning (if character was not found)
        usleep(5);
        setADL(i+1, LOW);
        setADC(2, LOW);            // TODO: Use 'j+1' instead of '2' as soon as ADC-interface works correctly
      }
    }
  }
}

//   chrono::steady_clock::time_point begin = chrono::steady_clock::now();
//   chrono::steady_clock::time_point end = chrono::steady_clock::now();
//   cout << endl << "  Waited for valid position for " << chrono::duration_cast<chrono::microseconds>(end - begin).count() << "[µs]" << endl;
