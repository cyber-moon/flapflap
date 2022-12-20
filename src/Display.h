#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <string>
#include <iostream>
#include <unistd.h>


// #include <Webserver.h>


using namespace std;



class Display {
private:

	static char supportedCharacters[];

	static const int START;
	static const int DATA[];
	static const int ADL[];
	static const int ADC[];

	string getCurrentBinaryCode();
public:
	Display();


	string reviseText(string text);
	// Screen Size
	static const int numOfRows;
	static const int numOfCols;



	void setSTART(uint8_t val);
	void setADL(int row, uint8_t val);
	void selectADC(int col);
	char getCurrentChar();
	// void printText(vector<string>& AAAAAAAAAAAAAAAAAAAAAA, Webserver& webserver);
	// void printText();
	void printText(vector<string>& AAAAAAAAAAAAAAAAAAAAAA);
};

#endif