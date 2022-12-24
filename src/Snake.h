#ifndef SNAKE_H
#define SNAKE_H

#include <Arduino.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <Display.h>

using namespace std;

class Snake {
private:
	Display flapflap;


public:
	inline static bool isRunning = false;
	int direction = 2;	// 0: left, 1: down, 2: right, 3: up

	vector<pair<int, int>> snake;

	Snake() {
	}

	void start() {
		snake = {{1, 3}, {1, 4}, {1, 5}};
		cout << "Printing Snake: ";
		for (pair<int, int> element : snake) {
			cout << element.first << ":" << element.second << " | ";
			flapflap.setADL(element.first, HIGH);
			flapflap.selectADC(element.second);
			flapflap.setSTART(HIGH);
			usleep(5);
			flapflap.setADL(element.first, LOW);
			flapflap.selectADC(31);
		}
		isRunning = true;
	}

	void move() {
		int horizontalMovement = (direction-1)%2;
		int verticalMovement = (direction-2)%2;

		pair<int, int> oldTail = snake.front();
		pair<int, int> oldHead = snake.back();
		snake.erase(snake.begin());
		snake.push_back({oldHead.first+verticalMovement, oldHead.second+horizontalMovement});

		pair<int, int> newHead = snake.back();

		cout << "New Snake: " << snake[0].first << ":" << snake[0].second << " | " << snake[1].first << ":" << snake[1].second << " | " << snake[2].first << ":" << snake[2].second << endl;

		// Start new head
		cout << "Starting: " << newHead.first << ":" << newHead.second << endl;
		flapflap.setADL(newHead.first, HIGH);
		flapflap.selectADC(newHead.second);
		flapflap.setSTART(HIGH);
		usleep(5);
		flapflap.setADL(newHead.first, LOW);
		flapflap.selectADC(31);

		// Stop old tail
		cout << "Stopping: " << oldTail.first << ":" << oldTail.second << endl;

		// char currentChar = '+';
		// while (!flapflap.isSupportedChar(currentChar)) {
		flapflap.setADL(oldTail.first, HIGH);
		flapflap.selectADC(oldTail.second);
		usleep(5);
		flapflap.setSTART(LOW);
		usleep(5);
		flapflap.setADL(oldTail.first, LOW);
		flapflap.selectADC(31);
		usleep(5);
		// 	currentChar = flapflap.getCurrentChar();
		// }

	}


};

#endif