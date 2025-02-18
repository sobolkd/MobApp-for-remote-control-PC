#ifndef MONITOR_BRIGHTNESS_H
#define MONITOR_BRIGHTNESS_H

#include <windows.h>
#include <highlevelmonitorconfigurationapi.h>
#include <iostream>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "dxva2.lib")

bool setMonitorBrightness(int brightness);

#endif // MONITOR_BRIGHTNESS_H
