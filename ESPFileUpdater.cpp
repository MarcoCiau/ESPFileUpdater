#include "ESPFileUpdater.h"
#include "FS.h"

#define USE_SPIFFS

#define DBG_OUTPUT_PORT Serial 

#define FILE_NAME "profile_1.txt"
static File fsUploadFile;
static ESPFileUpdater *self;

const char rootHTML[] PROGMEM =
"<!DOCTYPE html><html><head><meta http-equiv=Content-Type content=\"text/html; charset=utf-8\"><title>ESP File Updater</title><meta name=viewport content=\"width=device-width, user-scalable=no\"><style type=text/css>body{font-family: Sans-serif; display: table-cell; vertical-align: middle;} html, body{height:100%%;} html{display: table; margin: auto;}</style></head><body><h3>Upload File</h3><small><p>Select .text file and press \"Upload\".</p></small><form enctype=multipart/form-data action=/upload method=POST><input type=file accept=.txt name=data><input type=submit value=Upload></form></body></html>"
;


void handleRoot()
{
  self->server->send(200, "text/html", FPSTR(rootHTML));
}

void handleFileUpload(){
  if(self->server->uri() != "/upload") return;
  HTTPUpload& upload = self->server->upload();
  if(upload.status == UPLOAD_FILE_START){
    SPIFFS.remove(FILE_NAME);
    fsUploadFile = SPIFFS.open(FILE_NAME, "w");
  } else if(upload.status == UPLOAD_FILE_WRITE){
    DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile)
      fsUploadFile.close();
    DBG_OUTPUT_PORT.print("handleFileUpload Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
  }
}

ESPFileUpdater::ESPFileUpdater()
{
    self = this;
}

void ESPFileUpdater::begin()
{
#ifdef USE_SPIFFS
    SPIFFS.begin();
    Dir dir = SPIFFS.openDir("");
    DBG_OUTPUT_PORT.println(F("List of files at root of filesystem:"));
    while (dir.next()) 
    {
        String fileInfo = dir.fileName() + (dir.isDirectory() ? " [DIR]" : String(" (") + dir.fileSize() + "b)");
        DBG_OUTPUT_PORT.println(fileInfo);
    }
    DBG_OUTPUT_PORT.println();
#endif

  server = new ESP8266WebServer(80);
  server->on("/", handleRoot);
  server->on("/upload", HTTP_POST, [](){ self->server->send(200, "text/plain", "OK"); }, handleFileUpload);
  server->onNotFound([](){ self->server->send(404, "text/plain", "Not Found"); });
  server->begin();
}

void ESPFileUpdater::loop()
{
    server->handleClient();
}