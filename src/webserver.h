#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <string>
#include <iostream>
#include <unistd.h>

using namespace std;

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <Display.h>

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

  Display disp;
  // static void printing (Display& myDisp, vector<string>& text);


public:
	// Webserver(Display disp);
	Webserver();
  bool isDraftChange(int i);
  string getDraft(int i);
};

#endif