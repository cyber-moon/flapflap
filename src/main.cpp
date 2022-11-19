/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/vs-code-platformio-ide-esp32-esp8266-arduino/
*********/

#include <Arduino.h>
#include <string>
#include <iostream>
using namespace std;

#define START 22
#define DATA_0 32
#define DATA_1 33
#define DATA_2 34
#define DATA_3 35
#define DATA_4 36
#define DATA_5 39

int ADL[] = {25, 26, 27, 4};
int ADC[] = {21, 19, 18, 17, 16};

// l: 1, 2, 3 or 4
// val: HIGH or LOW
void setADL(int l, uint8_t val) {
  digitalWrite(ADL[l-1], val);
}

// c: Column-number ([0-n])
void selectADC(int c) {
  digitalWrite(ADC[4], (c/16 == 1));
  c = c%16;
  digitalWrite(ADC[3], (c/8 == 0));
  c = c%8;
  digitalWrite(ADC[2], (c/4 == 0));
  c = c%4;
  digitalWrite(ADC[1], (c/2 == 0));
  c = c%2;
  digitalWrite(ADC[0], (c/1 == 0));
}

void selectAllModules() {
  for (int i=0; i<=3; i++) {
    digitalWrite(ADL[i], HIGH);
  }
  for (int i=0; i<=4; i++) {
    digitalWrite(ADC[i], HIGH);
  }
}

void deselectAllModules() {
  for (int i=0; i<=3; i++) {
    digitalWrite(ADL[i], LOW);
  }
  for (int i=0; i<=4; i++) {
    digitalWrite(ADC[i], LOW);
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(START, OUTPUT);
  for (int i=0; i<=3; i++) {
    pinMode(ADL[i], OUTPUT);
  }
  for (int i=0; i<=4; i++) {
    pinMode(ADC[i], OUTPUT);
  }
  pinMode(DATA_0, INPUT);
  pinMode(DATA_1, INPUT);
  pinMode(DATA_2, INPUT);
  pinMode(DATA_3, INPUT);
  pinMode(DATA_4, INPUT);
  pinMode(DATA_5, INPUT);

  digitalWrite(START, LOW);
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
  if (binaryCode == "000000") {
    return '+'; // just return some non-existent value
  } 
  int intCode = (binaryCode[5-0]-'0')*1 + (binaryCode[5-1]-'0')*2 + (binaryCode[5-2]-'0')*4 + (binaryCode[5-3]-'0')*8 + (binaryCode[5-4]-'0')*16 + (binaryCode[5-5]-'0')*32;
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

int numOfCols = 4;

string* createText(string* text, int numOfRows) {
  for (int i=0; i<numOfRows; i++) {
    string line = text[i];
    for (int j=0; j<size(line); j++) {
      line[j] = toupper(line[j]);
    }
    if (size(line) < numOfCols) {
      int missingSpaces = numOfCols - size(line);
      char spaces[missingSpaces];
      fill_n(spaces, missingSpaces, ' ');
      line = line.append(spaces);
    }
    text[i] = line;
  }
  return text;
}

char getPrecedingCharacter(char myChar) {
  char characters[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','V','O','P','Q','R','S','T','U','V','W','X','Y','Z','/','-','1','2','3','4','5','6','7','8','9','0','.',' '};
  char* position = find(characters, characters+size(characters), myChar);
  char precedingChar = *(position-1);
  if (position == characters) {
    precedingChar = ' ';
  }
  return precedingChar;
}


void loop() {
  string draft[] = {"efgh"};
  int numOfRows = size(draft);
  string *text = createText(draft, numOfRows);

  char lastOutputChar[numOfRows * numOfCols];
  bool isCorrect[numOfRows * numOfCols];
  for (int i=0; i<numOfRows * numOfCols; i++) {
    isCorrect[i] = false;
  }

  // Iterate through the Matrix (Row-wise)
  while(1) {
    for (int i=0; i<numOfRows; i++) {
      string line = text[i];
      for (int j=0; j<numOfCols; j++) {
        // if (!isCorrect[2*i + j]) {
          char myChar = line[j];
          char precedingChar = getPrecedingCharacter(myChar);

          // Try to stop the motor
          digitalWrite(START, LOW);
          usleep(5);
          selectADC(j);
          setADL(i+1, HIGH);

          // if a valid character was detected
          char currentChar = getCurrentChar();
          // if(currentChar == myChar && lastOutputChar[2*i + j] == precedingChar) {
          if(currentChar == myChar) {
            digitalWrite(START, LOW);
            isCorrect[2*i + j] = true;
          } else {
            digitalWrite(START, HIGH);
          }
          if (currentChar != '+') {
            if (currentChar != lastOutputChar[2*i + j]) {
              cout << "Row: " << i << " Col: " << j << " Should: " << myChar << " Is: " << currentChar << endl;
              lastOutputChar[2*i + j] = currentChar;
            }
          }

          // Un-select the module, so it continues turning (if character was not found)
          usleep(5);
          setADL(i+1, LOW);
        // }
      }
    } 
  }

}