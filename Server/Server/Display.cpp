#include <windows.h>
#include <highlevelmonitorconfigurationapi.h>
#include <iostream>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "dxva2.lib")

bool setMonitorBrightness(int brightness) {
    if (brightness < 0 || brightness > 100) {
        std::cerr << "Brightness must be between 0 and 100." << std::endl;
        return false;
    }

    HMONITOR hMonitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);

    DWORD dwMonitorCount = 0;
    if (GetPhysicalMonitorsFromHMONITOR(hMonitor, dwMonitorCount, nullptr) == 0) {
        std::cerr << "Failed to get monitor count. Error code = " << GetLastError() << std::endl;
        return false;
    }

    if (dwMonitorCount == 0) {
        std::cerr << "No physical monitors found." << std::endl;
        return false;
    }

    LPPHYSICAL_MONITOR pMonitors = new PHYSICAL_MONITOR[dwMonitorCount];

    if (GetPhysicalMonitorsFromHMONITOR(hMonitor, dwMonitorCount, pMonitors) == 0) {
        std::cerr << "Failed to get monitor handles. Error code = " << GetLastError() << std::endl;
        delete[] pMonitors;
        return false;
    }

    for (DWORD i = 0; i < dwMonitorCount; ++i) {
        if (!SetMonitorBrightness(pMonitors[i].hPhysicalMonitor, brightness)) {
            std::cerr << "Failed to set brightness for monitor " << i + 1 << ". Error code = " << GetLastError() << std::endl;
        }
        else {
            std::cout << "Brightness set to " << brightness << "% for monitor " << i + 1 << std::endl;
        }
    }

    delete[] pMonitors;
    return true;
}
