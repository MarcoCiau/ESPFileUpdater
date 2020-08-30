#ifndef FILE_UPDATER_H
#define FILE_UPDATER_H
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

class ESPFileUpdater
{
private:
public:
    ESPFileUpdater();
    ESP8266WebServer *server;
    void begin();
    void loop();
};
#endif //class