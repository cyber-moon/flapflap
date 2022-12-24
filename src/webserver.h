#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <Display.h>

using namespace std;

#ifndef WEBSERVER_H
#define WEBSERVER_H


class Webserver {
private:
  // WiFi Credentials
  static const char* ssid;
  static const char* password;
  static AsyncWebServer server;

  // HTML web page
  static const char index_html[] PROGMEM;
  void registerHandlers();

  // Flapflap-Display
  Display flapflap;
	vector<string> draft;

public:
	Webserver();
};

#endif