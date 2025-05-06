#ifndef MONITOR_BRIGHTNESS_H
#define MONITOR_BRIGHTNESS_H

#include <windows.h>
#include <powrprof.h>
#include <iostream>

#pragma comment(lib, "PowrProf.lib")

int getMonitorBrightness();
bool setMonitorBrightness(int brightness);
bool setScreenResolution(int width, int height);
bool setQuietMode(bool enable);
bool setDisplayOrientation(DWORD orientation);
bool setMonitorSleep();
bool wakeUpMonitor();
#endif
