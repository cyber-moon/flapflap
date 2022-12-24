#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <Text.h>

using namespace std;

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <Snake.h>

class Webserver {
private:
  // WiFi Credentials
  static const char* ssid;
  static const char* password;
  static AsyncWebServer server;

  // HTML web page
  static const char index_html[] PROGMEM;
  static const char snake_html[] PROGMEM;
  void registerHandlers();

  // Flapflap-Display
  Text printer;
  Snake snake;
public:
	Webserver();
};

#endif