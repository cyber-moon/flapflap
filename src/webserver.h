#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <pthread.h>

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
  void registerHandlers();

  // Create a pthread_t variable to hold the thread handle
  pthread_t threadHandle;
  // Set up a pthread_attr_t structure to specify the stack size
  pthread_attr_t attr;

  // Flapflap-Display
	vector<string> draft;
  void asyncPrint();
  static void* printing (void* args);

public:
	Webserver();
};

#endif