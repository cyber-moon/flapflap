#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <pthread.h>

using namespace std;


class Display {
private:
	static char supportedCharacters[];

	static const int START;
	static const int DATA[];
	static const int ADL[];
	static const int ADC[];

	static void setSTART(uint8_t val);
	static void setADL(int row, uint8_t val);
	static void selectADC(int col);
	
	static string getCurrentBinaryCode();
	static char getCurrentChar();
	static string reviseText(string text);

	inline static bool printInProgress = false;
	inline static bool doRestart = false;

	inline static vector<string> revisedText;

public:
	pthread_t threadHandle;		// hold the thread handle
	pthread_attr_t attr;		// necessary for specifying the stack size

	Display();

	// Screen Size
	static const int numOfRows;
	static const int numOfCols;

	void updateDraft(vector<string> text);
	void asyncPrint();
	static void* printing (void* args);
	static void printText();
};

#endif