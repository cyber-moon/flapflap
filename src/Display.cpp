#include "Display.h"

/////////////////////////////////// PRIVATE ///////////////////////////////////////

const int Display::START = 22;
const int Display::DATA[] = {32, 33, 34, 35, 36, 39};
const int Display::ADL[] = {25, 26, 27, 4};
const int Display::ADC[] = {21, 19, 18, 17, 16};

/**
 * @returns String containing binary code of currently selected module
*/
string Display::getCurrentBinaryCode() {
  string binaryCode = "";
  for (int i=5; i>=0; i--) {
    binaryCode.append(to_string(!digitalRead(DATA[i])));
  }
  return binaryCode;
}

/////////////////////////////////// PUBLIC ///////////////////////////////////////

Display::Display() {
	// Define IO-Pinmode
	pinMode(START, OUTPUT);
	for (int i=0; i<=5; i++) {
		pinMode(DATA[i], INPUT);
	}
	for (int i=0; i<=3; i++) {
		pinMode(ADL[i], OUTPUT);
	}
	for (int i=0; i<=4; i++) {
		pinMode(ADC[i], OUTPUT);
	}
}

/**
 * Start (HIGH) or Stop (LOW) the next-selected Module
 * @param val HIGH or LOW
*/
void Display::setSTART(uint8_t val) {
	digitalWrite(START, val);
}

/**
 * Select (HIGH) or deselect (LOW) each ADL-Pin (i.e. each row)
 * @param row 0, 1, 2 or 3
 * @param val HIGH or LOW
*/
void Display::setADL(int row, uint8_t val) {
	digitalWrite(ADL[row], val);
}

/**
 * Select a given column number c.
 * Automatically un-selects all other columns.
 * @param col Column-number ([0-n]), n_max=31
 *            c=0: Modul on the very left (--> ADC=01111 (ADC[3]-ADC[0] are inverted))
 *            c=31: Modul on the very right (--> ADC=10000)
*/
void Display::selectADC(int col) {
	digitalWrite(ADC[4], (col/16 == 1));  // ADC[4] refers to the EN pin of the Multiplexer-chip, thus it is inverted
	col = col%16;
	digitalWrite(ADC[3], (col/8 == 0));
	col = col%8;
	digitalWrite(ADC[2], (col/4 == 0));
	col = col%4;
	digitalWrite(ADC[1], (col/2 == 0));
	col = col%2;
	digitalWrite(ADC[0], (col/1 == 0));
}

/**
 * @returns Character of currently selected module
*/
char Display::getCurrentChar() {
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
