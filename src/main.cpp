/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/vs-code-platformio-ide-esp32-esp8266-arduino/
*********/

#include <Arduino.h>
#include <string>
#include <iostream>
using namespace std;

#define START 22
#define ADL_1 25
#define ADL_2 26
#define ADL_3 27
#define ADL_4 4
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

void deselectAllModules() {
  digitalWrite(ADL_1, LOW);
  digitalWrite(ADL_2, LOW);
  digitalWrite(ADL_3, LOW);
  digitalWrite(ADL_4, LOW);
  digitalWrite(ADC_A, LOW);
  digitalWrite(ADC_B, LOW);
  digitalWrite(ADC_C, LOW);
  digitalWrite(ADC_D, LOW);
  digitalWrite(ADC_E, LOW);
}


void setup() {
  Serial.begin(115200);
  pinMode(ADL_1, OUTPUT);
  pinMode(ADL_2, OUTPUT);
  pinMode(ADL_3, OUTPUT);
  pinMode(ADL_4, OUTPUT);
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
  int ascii = 64 + (binaryCode[5-0]-'0')*1 + (binaryCode[5-1]-'0')*2 + (binaryCode[5-2]-'0')*4 + (binaryCode[5-3]-'0')*8 + (binaryCode[5-4]-'0')*16 + (binaryCode[5-5]-'0')*32;

  return char(ascii);
}

void loop() {
  string display = "BIGNA";

  for (int i=0; i<display.size(); ) {
    deselectAllModules();
    
    // Select Module 1
    digitalWrite(START, LOW);
    delay(1);
    digitalWrite(ADL_2, HIGH);
    digitalWrite(ADL_1, HIGH);

    // Serial.print("Selected!\n");
    if(!(getCurrentPosition() == "000000")) {
      delay(1);

      cout << getCurrentChar() << endl;
      
      if(getCurrentChar() == display[i]) {
        digitalWrite(START, LOW);
        Serial.print("Position found!\n");
        i++;
        delay(500);
      } else
      {
        digitalWrite(START, HIGH);
        Serial.print("still searching...\n");
      }
    } else 
    {
        digitalWrite(START, HIGH);
    }
    
    digitalWrite(ADL_1, LOW);
    delay(1);
    digitalWrite(ADL_2, LOW);



    digitalWrite(ADC_E, LOW);
    digitalWrite(ADC_D, LOW);
    digitalWrite(ADC_C, LOW);
    digitalWrite(ADC_B, LOW);
    digitalWrite(ADC_A, HIGH);

  }

}