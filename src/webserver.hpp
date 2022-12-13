#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <string>
#include <iostream>
#include <unistd.h>
using namespace std;


// string draft[] = {"", "flapflap.ch", ""};
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
	draft.clear();
	draft.push_back(line1Message.c_str());
	draft.push_back(line2Message.c_str());
	draft.push_back(line3Message.c_str());
//    draft[0] = line1Message.c_str();
//    draft[1] = line2Message.c_str();
//    draft[2] = line3Message.c_str();
    request->redirect("/");
  });
  server.onNotFound(notFound);
  server.begin();
}

bool isDraftChange(int i) {
	cout << "Draft size: " << draft.size() << endl;

	if (draft.size() >= i+1) {
		if (oldDraft.size() == 0) {
			oldDraft = draft;
			return true;
		}
		else if (oldDraft[0] != draft[0] || oldDraft[1] != draft[1] || oldDraft[2] != draft[2]) {
			oldDraft = draft;
			return true;
		}
	}
	return false;
}

string getDraft(int i) {
	if (draft.size() >= i+1) {
		cout << "returned: " << draft[i] << endl;
		return draft[i];
	}
	return "";
}
