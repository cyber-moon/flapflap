#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <string>
#include <iostream>
#include <unistd.h>

using namespace std;


struct Display {
	Display();
	static char supportedCharacters[];

	static const int START;
	static const int DATA[];
	static const int ADL[];
	static const int ADC[];

	static void setSTART(uint8_t val);
	static void setADL(int row, uint8_t val);
	static void selectADC(int col);
	
	static bool isSupportedChar(char x);

	static string getCurrentBinaryCode();
	static char getCurrentChar();

	static string reviseText(string text);

	// Screen Size
	static const int numOfRows;
	static const int numOfCols;
};

#endif