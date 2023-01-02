#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <Text.h>
#include <random>

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
  Text printer;

  // Knowledge generator
  static vector<vector<string>> knowledge;
  static random_device rd;
  static mt19937 gen; 
  static uniform_int_distribution<int> dist;

public:
	Webserver();
};

#endif