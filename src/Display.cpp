#include "Display.h"

using namespace std;


/////////////////////////////////// PRIVATE ///////////////////////////////////////

// Define Screen Size
const int Display::numOfRows = 3;
const int Display::numOfCols = 16;

// TODO: Why can't I do this STATIC & CONST?
char Display::supportedCharacters[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','V','O','P','Q','R','S','T','U','V','W','X','Y','Z','/','-','1','2','3','4','5','6','7','8','9','0','.',' '};

const int Display::START = 22;
const int Display::DATA[] = {32, 33, 34, 35, 36, 39};
const int Display::ADL[] = {25, 26, 27, 4};
const int Display::ADC[] = {21, 19, 18, 17, 16};

/**
 * Start (HIGH) or Stop (LOW) the next-selected Module
 * @param val HIGH or LOW
*/
void Display::setSTART(uint8_t val) {
	digitalWrite(START, val);
}

/**
 * Select (HIGH) or deselect (LOW) each ADL-Pin (i.e. each row)
 * @param row 0, 1, 2 or 3
 * @param val HIGH or LOW
*/
void Display::setADL(int row, uint8_t val) {
	digitalWrite(ADL[row], val);
}

/**
 * Select a given column number c.
 * Automatically un-selects all other columns.
 * @param col Column-number ([0-n]), n_max=31
 *            c=0: Modul on the very left (--> ADC=01111 (ADC[3]-ADC[0] are inverted))
 *            c=31: Modul on the very right (--> ADC=10000)
*/
void Display::selectADC(int col) {
	digitalWrite(ADC[4], (col/16 == 1));  // ADC[4] refers to the EN pin of the Multiplexer-chip, thus it is inverted
	col = col%16;
	digitalWrite(ADC[3], (col/8 == 0));
	col = col%8;
	digitalWrite(ADC[2], (col/4 == 0));
	col = col%4;
	digitalWrite(ADC[1], (col/2 == 0));
	col = col%2;
	digitalWrite(ADC[0], (col/1 == 0));
}

/**
 * @returns String containing binary code of currently selected module
*/
string Display::getCurrentBinaryCode() {
  string binaryCode = "";
  for (int i=5; i>=0; i--) {
    binaryCode.append(to_string(!digitalRead(DATA[i])));
  }
  return binaryCode;
}

/**
 * @returns Character of currently selected module
*/
char Display::getCurrentChar() {
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

/**
 * Remove unsupported Characters and, if necessary, add spaces at the end of line
 * @param text  single row of text, possibly containing unsupported characters (e.g. "fräch!")
 * @returns     single row of text, with characters replaced an missing spaces (e.g. "fr/ch/     ")
*/
string Display::reviseText(string text) {
  char* supportedCharacters_end = supportedCharacters+size(supportedCharacters);
  for (int j=0; j<size(text); j++) {
    text[j] = toupper(text[j]);
    if (find(supportedCharacters, supportedCharacters_end, text[j]) == supportedCharacters_end) {
      cout << "Unsupported character: " << text[j] << endl;
      text[j] = '/';
    }
  }
  if (size(text) < numOfCols) {
    int missingSpaces = numOfCols - size(text);
    char spaces[missingSpaces];
    fill_n(spaces, missingSpaces, ' ');
    text = text.append(spaces);
  }
  return text;
}

/////////////////////////////////// PUBLIC ///////////////////////////////////////

Display::Display() {
	// Define IO-Pinmode
	pinMode(START, OUTPUT);
	for (int i=0; i<=5; i++) {
		pinMode(DATA[i], INPUT);
	}
	for (int i=0; i<=3; i++) {
		pinMode(ADL[i], OUTPUT);
	}
	for (int i=0; i<=4; i++) {
		pinMode(ADC[i], OUTPUT);
	}

	// Set up Multithreading
	const size_t kStackSize = 100 * 1024; // 100 KB
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, kStackSize);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  printInProgress = false;
}

void Display::updateDraft(vector<string> text) {
  revisedText.clear();

  // Ensure that input vector matches the number of Rows
  while (text.size() < numOfRows) {
    text.push_back("");
  }

  for (auto line : text) {
    revisedText.push_back(reviseText(line));
  }
}

void Display::asyncPrint() {
  if (!printInProgress) {
    printInProgress = true;
    int create_success = pthread_create(&threadHandle, &attr, &printing, &revisedText);
    if (create_success != 0) 	throw logic_error("Creation of new thread failed.");
  } else {
    doRestart = true;
  }
  
}


void* Display::printing (void* args) {
	cout << "Printing text with seperate Thread" << endl;
	std::vector<std::string>* text = static_cast<std::vector<std::string>*>(args);

	printText(*text);
	return nullptr;
}

/**
 * Print the given string-vector (1 line per vector-element)
 * @param text	Vector containing num_of_rows strings with num_of_cols characters each
*/
void Display::printText(vector<string>& text) {
  cout << "printText: " << revisedText[0] << "   " << endl;

  // A Module x is in correct position if isCorrect[x]=10
	int isCorrect[numOfRows * numOfCols];
  fill_n(isCorrect, numOfRows * numOfCols, 0);

  // If all modules are correct, sum == 10*numOfRows*numOfCols
  int sum = 0;

	while(sum < 10*numOfCols*numOfRows) {
    // TODO: Stop all modules after 12s and exit while-loop

    sum = 0;
    // Iterate through the Matrix (Row-wise)
		for (int i=0; i<numOfRows; i++) {
			string line = revisedText[i];
			for (int j=0; j<numOfCols; j++) {
				// Correct char needs to be recognized in 10 iterations in a row to be valid
				if (isCorrect[numOfCols*i + j] < 10) {
          char myChar = line[j];

          // Try to stop the motor by de-activating START and selecting the module
          setSTART(LOW);
          usleep(5);
          selectADC(j);
          setADL(i, HIGH);

          // Stop if myChar is found (set START to LOW)
          char currentChar = getCurrentChar();
          if(currentChar == myChar) {
            setSTART(LOW);
            isCorrect[numOfCols*i + j]++;
          } else {
            setSTART(HIGH);
            isCorrect[numOfCols*i + j] = 0;
          }

          // Un-select the module, so it continues turning (if character was not found)
          usleep(5);
          setADL(i, LOW);
          selectADC(31);
				}
			}
		} 
    if (doRestart) {
      fill_n(isCorrect, numOfRows * numOfCols, 0);
      doRestart = false;
    }

    for (int moduleScore: isCorrect) {
      sum += moduleScore;
    }
	}
  printInProgress = false;
  pthread_exit(NULL);
}

