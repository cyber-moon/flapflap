#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <string>
#include <iostream>
#include <unistd.h>

using namespace std;


class Webserver {
private:
  // WiFi Credentials
  static const char* ssid;
  static const char* password;
  static AsyncWebServer server;

  // HTML web page
  static const char index_html[] PROGMEM;

  vector<string> draft;
  vector<string> oldDraft;
  void registerHandlers();

public:
	Webserver();
  bool isDraftChange(int i);
  string getDraft(int i);
};
