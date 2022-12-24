#ifndef TEXT_H
#define TEXT_H

#include <Arduino.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <Display.h>

using namespace std;


class Text {
private:
	static string getCurrentBinaryCode();
	static char getCurrentChar();

	inline static bool printInProgress = false;
	inline static bool doRestart = false;

	inline static vector<string> revisedText;

	static Display flapflap;

	pthread_t threadHandle;		// hold the thread handle
	pthread_attr_t attr;		// necessary for specifying the stack size
public:
	Text();

	void updateDraft(vector<string> text);
	void asyncPrint();
	static void* printing (void* args);
	static void printText();
};

#endif