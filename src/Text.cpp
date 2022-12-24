#include "Text.h"

using namespace std;


/////////////////////////////////// PRIVATE ///////////////////////////////////////

/**
 * @returns String containing binary code of currently selected module
*/
string Text::getCurrentBinaryCode() {
  string binaryCode = "";
  for (int i=5; i>=0; i--) {
    binaryCode.append(to_string(!digitalRead(flapflap.DATA[i])));
  }
  return binaryCode;
}

/**
 * @returns Character of currently selected module
*/
char Text::getCurrentChar() {
  string binaryCode = getCurrentBinaryCode();
  if (binaryCode == "000000") {
    return '+'; // just return some non-existent value
  } 
  int intCode = (binaryCode[5]-'0')*1 + (binaryCode[4]-'0')*2 + (binaryCode[3]-'0')*4 + (binaryCode[2]-'0')*8 + (binaryCode[1]-'0')*16 + (binaryCode[0]-'0')*32;

  // intCode 1-26: Characters (A-Z)
  // intCode 45-57: Numbers (0-9) and dash (-) and point (.)
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

/////////////////////////////////// PUBLIC ///////////////////////////////////////

Text::Text() {
	// Set up Multithreading
	const size_t kStackSize = 100 * 1024; // 100 KB
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, kStackSize);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  printInProgress = false;
}

/**
 * Update the object's revisedText-field, and revise the input text.
 * @param text  Vector containing a string for each line to be printed
*/
void Text::updateDraft(vector<string> text) {
  revisedText.clear();

  // Ensure that input vector matches the number of Rows
  while (text.size() < flapflap.numOfRows) {
    text.push_back("");
  }

  for (auto line : text) {
    revisedText.push_back(flapflap.reviseText(line));
  }
}

/**
 * Start the printing process
*/
void Text::asyncPrint() {
  if (!printInProgress) {
    printInProgress = true;
    int create_success = pthread_create(&threadHandle, &attr, &printing, NULL);
    if (create_success != 0) 	throw logic_error("Creation of new thread failed.");
  } else {
    doRestart = true;
  }
  
}

/**
 * Function handled to the seperate thread in asyncPrint.
 * Arguments and return values are not used.
*/
void* Text::printing (void* args) {
	printText();
	return nullptr;
}

/**
 * Print the given string-vector (1 line per vector-element)
 * @param text	Vector containing num_of_rows strings with num_of_cols characters each
*/
void Text::printText() {
  // A Module x is in correct position if isCorrect[x]=10
	int isCorrect[flapflap.numOfRows * flapflap.numOfCols];
  fill_n(isCorrect, flapflap.numOfRows * flapflap.numOfCols, 0);

  // If all modules are correct, allCorrect == 10*numOfRows*numOfCols
  int allCorrect = 0;

  // Exit while loop after 12s latest
  time_t exitTime = time(NULL) + 12;
  cout << time(NULL) << "     " << exitTime << endl;

	while(allCorrect < 10*flapflap.numOfCols*flapflap.numOfRows && time(NULL)<exitTime) {
    // Iterate through the Matrix (Row-wise)
		for (int i=0; i<flapflap.numOfRows; i++) {
			string line = revisedText[i];
			for (int j=0; j<flapflap.numOfCols; j++) {
				// Correct char needs to be recognized in 10 iterations in a row to be valid
				if (isCorrect[flapflap.numOfCols*i + j] < 10) {
          char myChar = line[j];

          // Try to stop the motor by de-activating START and selecting the module
          flapflap.setSTART(LOW);
          usleep(5);
          flapflap.selectADC(j);
          flapflap.setADL(i, HIGH);

          // Stop if myChar is found (set START to LOW)
          char currentChar = getCurrentChar();
          if(currentChar == myChar) {
            flapflap.setSTART(LOW);
            isCorrect[flapflap.numOfCols*i + j]++;
          } else {
            flapflap.setSTART(HIGH);
            isCorrect[flapflap.numOfCols*i + j] = 0;
          }

          // Un-select the module, so it continues turning (if character was not found)
          usleep(5);
          flapflap.setADL(i, LOW);
          flapflap.selectADC(31);
				}
			}
		} 

    // if a restart was triggered (from web handler), reset isCorrect to 0
    if (doRestart) {
      fill_n(isCorrect, flapflap.numOfRows * flapflap.numOfCols, 0);
      doRestart = false;
    }

    // Indirectly count the number of correct modules
    allCorrect = 0;
    for (int moduleScore: isCorrect) {
      allCorrect += moduleScore;
    }
	}

  // Ensure that all modules are stopped
  for (int i=0; i<flapflap.numOfRows; i++) {
    for (int j=0; j<flapflap.numOfCols; j++) {
        flapflap.setSTART(LOW);
        usleep(5);
        flapflap.selectADC(j);
        flapflap.setADL(i, HIGH);
    }
  } 

  printInProgress = false;
  pthread_exit(NULL);
}

