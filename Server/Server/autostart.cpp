#include "autostart.h"
#include <windows.h>
#include <string>

// add to autolaunch
bool addToAutostart(const std::wstring& appName, const std::wstring& appPath) {
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0, KEY_SET_VALUE, &hKey);
    if (result != ERROR_SUCCESS) {
        return false;
    }

    result = RegSetValueExW(hKey, appName.c_str(), 0, REG_SZ,
        reinterpret_cast<const BYTE*>(appPath.c_str()),
        static_cast<DWORD>((appPath.size() + 1) * sizeof(wchar_t)));

    RegCloseKey(hKey);
    return (result == ERROR_SUCCESS);
}

// delete from autolaunch
bool removeFromAutostart(const std::wstring& appName) {
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0, KEY_SET_VALUE, &hKey);
    if (result != ERROR_SUCCESS) {
        return false;
    }

    result = RegDeleteValueW(hKey, appName.c_str());
    RegCloseKey(hKey);

    return (result == ERROR_SUCCESS);
}
