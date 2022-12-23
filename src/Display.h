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

	inline static bool printInProgress;
	inline static bool doRestart = false;

	inline static vector<string> revisedText;

	// Create a pthread_t variable to hold the thread handle
	pthread_t threadHandle;
	// Set up a pthread_attr_t structure to specify the stack size
	pthread_attr_t attr;
public:
	Display();

	// Screen Size
	static const int numOfRows;
	static const int numOfCols;

	void updateDraft(vector<string> text);
	void asyncPrint();
	static void* printing (void* args);
	static void printText(vector<string>& text);
};

#endif