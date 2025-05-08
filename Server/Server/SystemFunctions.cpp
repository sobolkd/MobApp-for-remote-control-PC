#include <Windows.h>
#include "SystemFunctions.h"
#include <pdh.h>
#include <pdhmsg.h>

// restart PC
bool restartComputer() {
    try {
        HANDLE hToken;
        TOKEN_PRIVILEGES tkp;

        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
            return false;

        if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid))
            return false;

        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0))
            return false;

        if (GetLastError() != ERROR_SUCCESS)
            return false;

        if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MAJOR_SOFTWARE | SHTDN_REASON_FLAG_PLANNED))
            return false;

        return true;
    }
    catch (...) {
        return false;
    }
}
// shut down PC
bool shutdownComputer() {
    try {
        HANDLE hToken;
        TOKEN_PRIVILEGES tkp;

        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
            return false;

        if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid))
            return false;

        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0))
            return false;

        if (GetLastError() != ERROR_SUCCESS)
            return false;

        if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_MAJOR_SOFTWARE | SHTDN_REASON_FLAG_PLANNED))
            return false;

        return true;
    }
    catch (...) {
        return false;
    }
}
// Lock Screen
bool lockWorkstation() {
    try {
        return LockWorkStation() != 0;
    }
    catch (...) {
        return false;
    }
}
// End user session
bool logoffUser() {
    try {
        return ExitWindowsEx(EWX_LOGOFF, SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_FLAG_PLANNED) != 0;
    }
    catch (...) {
        return false;
    }
}
// call TaskManager
bool openTaskManager() {
    try {
        return (int)ShellExecuteW(NULL, L"open", L"taskmgr.exe", NULL, NULL, SW_SHOWDEFAULT) > 32;
    }
    catch (...) {
        return false;
    }
}
// Show Desktop
bool showDesktop() {
    try {
        keybd_event(VK_LWIN, 0, 0, 0);
        keybd_event('D', 0, 0, 0);
        keybd_event('D', 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
        return true;
    }
    catch (...) {
        return false;
    }
}
// Alt+Tab
bool altTab() {
    try {
        keybd_event(VK_MENU, 0, 0, 0);         
        keybd_event(VK_TAB, 0, 0, 0);          
        keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0); 
        keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
        return true;
    }
    catch (...) {
        return false;
    }
}
// Get Cpu Usage
double getCpuUsage() {
    static PDH_HQUERY cpuQuery;
    static PDH_HCOUNTER cpuTotal;
    static bool initialized = false;

    if (!initialized) {
        PdhOpenQuery(NULL, NULL, &cpuQuery);
        PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
        PdhCollectQueryData(cpuQuery);
        initialized = true;
        Sleep(100);
    }

    PdhCollectQueryData(cpuQuery);
    PDH_FMT_COUNTERVALUE counterVal;
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);

    return counterVal.doubleValue;
}
// Get Memory Usage
double getMemoryUsage() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(memInfo);
    GlobalMemoryStatusEx(&memInfo);

    DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
    DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;

    return (physMemUsed * 100.0) / totalPhysMem;
}
// Get Disk Usage
double getDiskUsage() {
    ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;

    if (GetDiskFreeSpaceEx(L"C:\\", &freeBytesAvailable, &totalBytes, &totalFreeBytes)) {
        ULONGLONG usedBytes = totalBytes.QuadPart - totalFreeBytes.QuadPart;
        return (usedBytes * 100.0) / totalBytes.QuadPart;
    }
    return -1.0;
}