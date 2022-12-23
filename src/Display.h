#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <string>
#include <iostream>
#include <unistd.h>

using namespace std;


class Display {
private:
	static char supportedCharacters[];

	static const int START;
	static const int DATA[];
	static const int ADL[];
	static const int ADC[];

	void setSTART(uint8_t val);
	void setADL(int row, uint8_t val);
	void selectADC(int col);
	
	string getCurrentBinaryCode();
	char getCurrentChar();
	string reviseText(string text);
public:
	Display();

	// Screen Size
	static const int numOfRows;
	static const int numOfCols;

	void printText(vector<string>& text);
};

#endif