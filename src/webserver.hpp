#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <string>
#include <iostream>
#include <unistd.h>
using namespace std;


vector<string> draft;
vector<string> oldDraft;

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
		<form action='/input' method='post'>
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
		<form action="/reset" method='post'>
      <div id="formbox">
        <input type="submit" value="reset" style="font-size : 35px; height:50px; width:150px">
      </div>
		</form><br>
		<form action="/xxx" method='post'>
      <div id="formbox">
        <input type="submit" value="XXX" style="font-size : 35px; height:50px; width:150px">
      </div>
		</form><br>
		<form action="/abc" method='post'>
      <div id="formbox">
        <input type="submit" value="ABC" style="font-size : 35px; height:50px; width:150px">
      </div>
		</form><br>
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

  server.on("/input", HTTP_POST, [] (AsyncWebServerRequest *request) {
    int params = request->params();
    draft.clear();
    for (int i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      draft.push_back(p->value().c_str());
    }
    request->redirect("/");
  });

  server.on("/reset", HTTP_POST, [] (AsyncWebServerRequest *request) {
    draft.clear();
    draft.push_back(" Willkommen. ");
    draft.push_back("Los gehts auf");
    draft.push_back(" flapflap.ch ");
    request->redirect("/");
  });

  server.on("/xxx", HTTP_POST, [] (AsyncWebServerRequest *request) {
    draft.clear();
    draft.push_back("xxxxxxxxxxxxx");
    draft.push_back("xxxxxxxxxxxxx");
    draft.push_back("xxxxxxxxxxxxx");
    request->redirect("/");
  });

  server.on("/abc", HTTP_POST, [] (AsyncWebServerRequest *request) {
    draft.clear();
    draft.push_back("ABCDEFGHIJKLM");
    draft.push_back("NOPQRSTUVWXYZ");
    draft.push_back("/-1234567890.");
    request->redirect("/");
  });

  server.onNotFound(notFound);
  server.begin();
}

bool isDraftChange(int i) {
	if (draft.size() >= i+1) {
		if (oldDraft.size() == 0) {
			oldDraft = draft;
			return true;
		}
		// TODO: Make dependent on number of rows, instead hardcoding 0, 1 and 2.
		else if (oldDraft[0] != draft[0] || oldDraft[1] != draft[1] || oldDraft[2] != draft[2]) {
			oldDraft = draft;
			return true;
		}
	}
	return false;
}

string getDraft(int i) {
	if (draft.size() >= i+1) {
		return draft[i];
	}
	return "";
}
