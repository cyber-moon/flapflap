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
		draft.clear();
		for (int i = 0; i < params; i++) {
			AsyncWebParameter* p = request->getParam(i);
			draft.push_back(p->value().c_str());
		}

		request->redirect("/");


		// TODO: Delete all old threads and stop Flapflap, before creating a new thread
		asyncPrint();
	});

	server.on("/reset", HTTP_POST, [&] (AsyncWebServerRequest *request) {
		draft.clear();
		draft.push_back(" Willkommen. ");
		draft.push_back("Los gehts auf");
		draft.push_back(" flapflap.ch ");
		request->redirect("/");
		asyncPrint();
	});

	server.on("/xxx", HTTP_POST, [&] (AsyncWebServerRequest *request) {
		draft.clear();
		draft.push_back("xxxxxxxxxxxxx");
		draft.push_back("xxxxxxxxxxxxx");
		draft.push_back("xxxxxxxxxxxxx");
		request->redirect("/");
		asyncPrint();
	});

	server.on("/abc", HTTP_POST, [&] (AsyncWebServerRequest *request) {
		draft.clear();
		draft.push_back("ABCDEFGHIJKLM");
		draft.push_back("NOPQRSTUVWXYZ");
		draft.push_back("/-1234567890.");
		request->redirect("/");
		asyncPrint();
	});

	server.onNotFound([] (AsyncWebServerRequest *request) {
		request->send(404, "text/plain", "Not found");
	});
}

void Webserver::asyncPrint() {
  	// pair<Display, vector<string>> params(disp, draft);
	int create_success = pthread_create(&threadHandle, &attr, &printing, &draft);
	if (create_success != 0) 	throw logic_error("Creation of new thread failed.");
}

void* Webserver::printing (void* args) {
	cout << "Printing text with seperate Thread" << endl;
	std::vector<std::string>* text = static_cast<std::vector<std::string>*>(args);
	Display disp;
	// pair<Display, vector<string>>* params = static_cast<std::pair<Display, std::vector<std::string>>*>(args);
	// Display disp = params->first;
	// std::vector<std::string> text = params->second;
	disp.printText(*text);
	return nullptr;
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

	// Set up Multithreading
	const size_t kStackSize = 100 * 1024; // 100 KB
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, kStackSize);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

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
		<input type="text" name="line1" maxlength="16" style="font-family: 'Courier New'; font-size : 28px; height:40px; width:260px"><br />
		<br>
		<input type="text" name="line2" maxlength="16" style="font-family: 'Courier New'; font-size : 28px; height:40px; width:260px"><br />
		<br>
		<input type="text" name="line3" maxlength="16" style="font-family: 'Courier New'; font-size : 28px; height:40px; width:260px"><br />
		<br>
		<input type="submit" value="flapflap" style="font-size : 35px; height:50px; width:150px">
	</div>
	</form><br>
	<center>
	Max. 16 Zeichen pro Zeile. <br>
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




