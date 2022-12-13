/*********
  Stephan Zehnder
  15.11.2022
  Controlling an Omega 3021 Flipflap-Display
*********/

#include <Arduino.h>
#include <string>
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
using namespace std;

#define START 22
#define DATA_0 32
#define DATA_1 33
#define DATA_2 34
#define DATA_3 35
#define DATA_4 36
#define DATA_5 39

int ADL[] = {25, 26, 27, 4};
int ADC[] = {21, 19, 18, 17, 16};
char supportedCharacters[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','V','O','P','Q','R','S','T','U','V','W','X','Y','Z','/','-','1','2','3','4','5','6','7','8','9','0','.',' '};

string draft[] = {"        ", "flap.local", "        "};

int numOfRows = 3;
int numOfCols = 13;

// Set up Webserver
AsyncWebServer server(80);
const char* ssid = "zimtbaum";
const char* password = "Zipfel-Muetze";

// HTML web page to handle input field
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
	<head>
		<title>flapflap</title>
		<meta name="viewport" content="width=device-width, initial-scale=1">
    <style type="text/css">
        #btn_s{
            width:100px;
        }

        #btn_i {
            width:125px;
        }
        #formbox {
            margin:auto 0;
            text-align: center;
        }
    </style>
  </head>
	<body>
		<form action="/get">
      <div id="formbox">
        <br>
        <input type="text" name="line1" maxlength="13" style="font-family: 'Courier New'; font-size : 28px; height:40px; width:260px"><br />
        <br>
        <input type="text" name="line2" maxlength="13" style="font-family: 'Courier New'; font-size : 28px; height:40px; width:260px"><br />
        <br>
        <input type="text" name="line3" maxlength="13" style="font-family: 'Courier New'; font-size : 28px; height:40px; width:260px"><br />
        <br>
        <input type="submit" value="flapflap" style="font-size : 35px; height:50px; width:150px">
      </div>
		</form><br>
    <center>
      Max. 13 Zeichen pro Zeile. <br>
      Erlaubte Zeichen: 'A'-'Z', '0'-'9', '.', '/', '-', ' '
    </center>
	</body>
</html>
)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setupWebserver() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  if(!MDNS.begin("flap")) {
    Serial.println("Error starting mDNS");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  string ipAdress = WiFi.localIP().toString().c_str();
  cout << ipAdress << endl;

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET input value on <ESP_IP>/get?input=<inputMessage>
    String line1Message = request->getParam("line1")->value();
    String line2Message = request->getParam("line2")->value();
    String line3Message = request->getParam("line3")->value();
    draft[0] = line1Message.c_str();
    draft[1] = line2Message.c_str();
    draft[2] = line3Message.c_str();
    request->redirect("/");
  });
  server.onNotFound(notFound);
  server.begin();
}




// l: 1, 2, 3 or 4
// val: HIGH or LOW
void setADL(int l, uint8_t val) {
  digitalWrite(ADL[l-1], val);
}

// c: Column-number ([0-n])
void selectADC(int c) {
  digitalWrite(ADC[4], (c/16 == 1));
  c = c%16;
  digitalWrite(ADC[3], (c/8 == 0));
  c = c%8;
  digitalWrite(ADC[2], (c/4 == 0));
  c = c%4;
  digitalWrite(ADC[1], (c/2 == 0));
  c = c%2;
  digitalWrite(ADC[0], (c/1 == 0));
}

void selectAllModules() {
  for (int i=0; i<=3; i++) {
    digitalWrite(ADL[i], HIGH);
  }
  for (int i=0; i<=4; i++) {
    digitalWrite(ADC[i], HIGH);
  }
}

void deselectAllModules() {
  for (int i=0; i<=3; i++) {
    digitalWrite(ADL[i], LOW);
  }
  for (int i=0; i<=3; i++) {
    digitalWrite(ADC[i], LOW);
  }
  digitalWrite(ADC[4], HIGH);
}

void setup() {
  Serial.begin(115200);
  setupWebserver();

  pinMode(START, OUTPUT);
  for (int i=0; i<=3; i++) {
    pinMode(ADL[i], OUTPUT);
  }
  for (int i=0; i<=4; i++) {
    pinMode(ADC[i], OUTPUT);
  }
  pinMode(DATA_0, INPUT);
  pinMode(DATA_1, INPUT);
  pinMode(DATA_2, INPUT);
  pinMode(DATA_3, INPUT);
  pinMode(DATA_4, INPUT);
  pinMode(DATA_5, INPUT);

  // Start all modules and then de-select them
  selectAllModules();
  digitalWrite(START, HIGH);
  deselectAllModules();
}

string getCurrentPosition() {
  string binaryCode = "";
  binaryCode.append(to_string(!!!digitalRead(DATA_5)));
  binaryCode.append(to_string(!!!digitalRead(DATA_4)));
  binaryCode.append(to_string(!!!digitalRead(DATA_3)));
  binaryCode.append(to_string(!!!digitalRead(DATA_2)));
  binaryCode.append(to_string(!!!digitalRead(DATA_1)));
  binaryCode.append(to_string(!!!digitalRead(DATA_0)));
  return binaryCode;
}

char getCurrentChar() {
  string binaryCode = getCurrentPosition();
  if (binaryCode == "000000") {
    return '+'; // just return some non-existent value
  } 
  int intCode = (binaryCode[5-0]-'0')*1 + (binaryCode[5-1]-'0')*2 + (binaryCode[5-2]-'0')*4 + (binaryCode[5-3]-'0')*8 + (binaryCode[5-4]-'0')*16 + (binaryCode[5-5]-'0')*32;
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

string reviseText(string text) {
  for (int j=0; j<size(text); j++) {
    text[j] = toupper(text[j]);
    if (find(supportedCharacters, supportedCharacters+size(supportedCharacters), text[j]) == (supportedCharacters+size(supportedCharacters))) {
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

char getPrecedingCharacter(char myChar) {
  char* position = find(supportedCharacters, supportedCharacters+size(supportedCharacters), myChar);
  char precedingChar = *(position-1);
  if (position == supportedCharacters) {
    precedingChar = ' ';
  }
  return precedingChar;
}


void loop() {
  string oldDraft[] = {"", "", ""};
  string text[] = {"", "", ""};
  string lastOutputChars[numOfRows * numOfCols];
  int isCorrect[numOfRows * numOfCols];

  // Iterate through the Matrix (Row-wise)
  while(1) {
    if (oldDraft[0] != draft[0] || oldDraft[1] != draft[1] || oldDraft[2] != draft[2]) {
      text[0] = reviseText(draft[0]);
      text[1] = reviseText(draft[1]);
      text[2] = reviseText(draft[2]);
      oldDraft[0] = draft[0];
      oldDraft[1] = draft[1];
      oldDraft[2] = draft[2];
      for (int i=0; i<numOfRows * numOfCols; i++) {
        lastOutputChars[i] = "";
        isCorrect[i] = 0;
      }
    }

    for (int i=0; i<numOfRows; i++) {
      string line = text[i];
      for (int j=0; j<numOfCols; j++) {
        if (isCorrect[numOfCols*i + j] < 10) {
          char myChar = line[j];
          char precedingChar = getPrecedingCharacter(myChar);

          // Try to stop the motor
          digitalWrite(START, LOW);
          usleep(5);
          selectADC(j);
          setADL(i+1, HIGH);

          // Stop if myChar is found
          char currentChar = getCurrentChar();
          if(currentChar == myChar) {
            digitalWrite(START, LOW);
            isCorrect[numOfCols*i + j]++;
          } else {
            digitalWrite(START, HIGH);
            isCorrect[numOfCols*i + j] = 0;
          }

          // Un-select the module, so it continues turning (if character was not found)
          usleep(5);
          setADL(i+1, LOW);
          selectADC(31);
        }
      }
    } 
  }
}

//   chrono::steady_clock::time_point begin = chrono::steady_clock::now();
//   chrono::steady_clock::time_point end = chrono::steady_clock::now();
//   cout << endl << "  Waited for valid position for " << chrono::duration_cast<chrono::microseconds>(end - begin).count() << "[µs]" << endl;
