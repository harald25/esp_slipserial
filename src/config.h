#ifndef __BLINK_H_INCLUDED_
#define __BLINK_H_INCLUDED_
#include <ESP8266WiFi.h>

void blink();
void blinkUpdate();

extern const char* ssid; //  your network name (SSID)
extern const char* pass; // your network password

extern IPAddress espip;  //IP of the ESP
extern IPAddress gateway;
extern IPAddress subnet;

#endif
