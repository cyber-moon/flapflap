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

// HTML web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
	<head>
		<title>flapflap</title>
		<meta name="viewport" content="width=device-width, initial-scale=1">
    <style type="text/css">
        #formbox {
            margin:auto 0;
            text-align: center;
        }
        .colform { 
          float:left; 
          width:33%; 
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
    </center><br>
    <center>
      <form class="colform" action="/reset" method='post'>
        <input type="submit" value="reset" style="font-size : 35px; height:50px; width:100px">
      </form>
      <form class="colform" action="/xxx" method='post'>
        <input type="submit" value="XXX" style="font-size : 35px; height:50px; width:100px">
      </form>
      <form class="colform" action="/abc" method='post'>
        <input type="submit" value="ABC" style="font-size : 35px; height:50px; width:100px">
      </form>
    </center><br>
	</body>
</html>
)rawliteral";

void setupWebserver() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  // Make server accessible over 'flap.local' in WLAN Network
  if(!MDNS.begin("flap")) {
    Serial.println("Error starting mDNS");
    return;
  }
  cout << "IP Address: " << WiFi.localIP().toString().c_str() << endl;

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Handle API Requests
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

  server.onNotFound([] (AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  });

  server.begin();
}

bool isDraftChange(int i) {
	if (draft.size() >= i+1) {
		if (oldDraft.size() == 0) {
			oldDraft = draft;
			return true;
		}
    for (int x=0; x<=i; x++) {
      if (oldDraft[x] != draft[x]) {
        oldDraft = draft;
        return true;
      }
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
