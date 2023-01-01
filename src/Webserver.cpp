#include <Webserver.h>

using namespace std;


/////////////////////////////////// PRIVATE ///////////////////////////////////////

// WiFi Credentials
const char* Webserver::ssid = "zimtbaum";
const char* Webserver::password = "Zipfel-Muetze";

AsyncWebServer Webserver::server = AsyncWebServer(80);

/**
 * Register Handlers for API Requests
*/
void Webserver::registerHandlers() {
	// Basic Homepage
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send_P(200, "text/html", index_html);
		cout << "printed default page" << endl;
	});

	// Handle API Requests
	server.on("/input", HTTP_POST, [&] (AsyncWebServerRequest *request) {
		int params = request->params();
		vector<string> draft;
		for (int i = 0; i < params; i++) {
			AsyncWebParameter* p = request->getParam(i);
			draft.push_back(p->value().c_str());
		}
		request->redirect("/");
		printer.updateDraft(draft);
		printer.asyncPrint();
	});

	server.on("/reset", HTTP_POST, [&] (AsyncWebServerRequest *request) {
		vector<string> draft;
		draft.push_back("  Willkommen. ");
		draft.push_back(" Los gehts auf");
		draft.push_back("  flapflap.ch ");
		request->redirect("/");
		printer.updateDraft(draft);
		printer.asyncPrint();
	});

	server.on("/xxx", HTTP_POST, [&] (AsyncWebServerRequest *request) {
		vector<string> draft;
		draft.push_back("xxxxxxxxxxxxxxxx");
		draft.push_back("xxxxxxxxxxxxxxxx");
		draft.push_back("xxxxxxxxxxxxxxxx");
		request->redirect("/");
		printer.updateDraft(draft);
		printer.asyncPrint();
	});

	server.on("/abc", HTTP_POST, [&] (AsyncWebServerRequest *request) {
		vector<string> draft;
		draft.push_back("ABCDEFGHIJKLMNOP");
		draft.push_back("QRSTUVWXYZ/-1234");
		draft.push_back("567890. ABCDEFGH");
		request->redirect("/");
		printer.updateDraft(draft);
		printer.asyncPrint();
	});

	server.onNotFound([] (AsyncWebServerRequest *request) {
		request->send(404, "text/plain", "Not found");
	});
}


/////////////////////////////////// PUBLIC ///////////////////////////////////////

Webserver::Webserver() {
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

	registerHandlers();
	server.begin();
}

// HTML web page
const char Webserver::index_html[] PROGMEM = R"rawliteral(
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
		<input type="text" name="line1" maxlength="18" style="font-family: 'Courier New'; font-size : 28px; height:40px; width:320px"><br />
		<br>
		<input type="text" name="line2" maxlength="18" style="font-family: 'Courier New'; font-size : 28px; height:40px; width:320px"><br />
		<br>
		<input type="text" name="line3" maxlength="18" style="font-family: 'Courier New'; font-size : 28px; height:40px; width:320px"><br />
		<br>
		<input type="submit" value="flapflap" style="font-size : 35px; height:50px; width:150px">
	</div>
	</form><br>
	<center>
	Max. 18 Zeichen pro Zeile. <br>
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

