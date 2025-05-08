#include <Windows.h>
#include "SystemFunctions.h"

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