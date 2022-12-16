#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <string>
#include <iostream>
#include <unistd.h>
using namespace std;



class Display {
private:
	static const int START;
	static const int DATA[];
	static const int ADL[];
	static const int ADC[];
	string getCurrentBinaryCode();
public:
	Display();
	void setSTART(uint8_t val);
	void setADL(int row, uint8_t val);
	void selectADC(int col);
	char getCurrentChar();
};

#endif