#ifndef TEXT_H
#define TEXT_H

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

	int direction = 0;	// 0: right, 1: down, 2: left, 3: up

public:
	vector<pair<int, int>> snake = {{1, 3}, {1, 4}, {1, 5}};

	Snake() {
		for (pair<int, int> element : snake) {
			flapflap.setSTART(HIGH);
			usleep(5);
			flapflap.selectADC(element.first);
			flapflap.setADL(element.second, HIGH);
		}
	}

	void move() {
		int horizontalMovement = (direction-1)%2;
		int verticalMovement = (direction-2)%2;

		pair<int, int> oldTail = snake.front();
		pair<int, int> oldHead = snake.back();
		snake.erase(snake.begin());
		snake.push_back({oldHead.first+verticalMovement, oldHead.second+horizontalMovement});

		pair<int, int> newHead = snake.back();

		// Start new head
		flapflap.setSTART(HIGH);
		usleep(5);
		flapflap.selectADC(newHead.first);
		flapflap.setADL(newHead.second, HIGH);

		// Stop old tail
		flapflap.setSTART(LOW);
		usleep(5);
		flapflap.selectADC(oldTail.first);
		flapflap.setADL(oldTail.second, HIGH);

	}


};

#endif