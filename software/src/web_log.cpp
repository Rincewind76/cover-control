#include "web_log.h"
#include <ESPAsyncWebServer.h>
#include <stdarg.h>

extern AsyncEventSource logEvents;

void webLog(const String& msg) {
    logEvents.send(msg.c_str(), "log");
    //Serial.println(msg);
}

void webLogf(const char* fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    webLog(String(buf));
}
