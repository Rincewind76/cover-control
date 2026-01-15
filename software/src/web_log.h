#pragma once
#include <Arduino.h>

void webLog(const String& msg);
void webLogf(const char* fmt, ...);

// Zentrale Logging-Makros
#define LOG(msg) webLog(msg)
#define LOGF(...) webLogf(__VA_ARGS__)