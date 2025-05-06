#include <windows.h>
#include <powrprof.h>
#include <iostream>
#include <atlbase.h>
#include <wrl.h>

#pragma comment(lib, "PowrProf.lib")

using namespace Microsoft::WRL;

int getMonitorBrightness() {
    DWORD brightness;
    GUID* activeScheme = nullptr;

    if (PowerGetActiveScheme(NULL, &activeScheme) != ERROR_SUCCESS) {
        std::cerr << "Failed to get active power scheme. Error code: " << GetLastError() << std::endl;
        return -1;
    }

    if (PowerReadACValueIndex(NULL, activeScheme, &GUID_VIDEO_SUBGROUP, &GUID_DEVICE_POWER_POLICY_VIDEO_BRIGHTNESS, &brightness) != ERROR_SUCCESS) {
        std::cerr << "Failed to read brightness. Error code: " << GetLastError() << std::endl;
        LocalFree(activeScheme);
        return -1;
    }

    LocalFree(activeScheme);
    return static_cast<int>(brightness);
}

bool setMonitorBrightness(int brightness) {
    if (brightness < 0 || brightness > 100) {
        std::cerr << "Brightness must be between 0 and 100." << std::endl;
        return false;
    }

    GUID* activeScheme;
    if (PowerGetActiveScheme(NULL, &activeScheme) != ERROR_SUCCESS) {
        std::cerr << "Failed to get active power scheme." << std::endl;
        return false;
    }

    if (PowerWriteACValueIndex(NULL, activeScheme, &GUID_VIDEO_SUBGROUP, &GUID_DEVICE_POWER_POLICY_VIDEO_BRIGHTNESS, brightness) != ERROR_SUCCESS ||
        PowerWriteDCValueIndex(NULL, activeScheme, &GUID_VIDEO_SUBGROUP, &GUID_DEVICE_POWER_POLICY_VIDEO_BRIGHTNESS, brightness) != ERROR_SUCCESS) {
        std::cerr << "Failed to set brightness." << std::endl;
        LocalFree(activeScheme);
        return false;
    }

    if (PowerSetActiveScheme(NULL, activeScheme) != ERROR_SUCCESS) {
        std::cerr << "Failed to apply brightness settings." << std::endl;
        LocalFree(activeScheme);
        return false;
    }

    LocalFree(activeScheme);
    return true;
}

bool setScreenResolution(int width, int height) {
    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    int modeIndex = 0;
    bool resolutionAvailable = false;

    while (EnumDisplaySettings(NULL, modeIndex, &devMode)) {
        if (devMode.dmPelsWidth == width && devMode.dmPelsHeight == height) {
            resolutionAvailable = true;
            break;
        }
        modeIndex++;
    }

    if (!resolutionAvailable) {
        std::cerr << "Requested resolution is not supported." << std::endl;
        return false;
    }

    devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

    LONG result = ChangeDisplaySettingsEx(NULL, &devMode, NULL, CDS_UPDATEREGISTRY | CDS_RESET, NULL);
    if (result != DISP_CHANGE_SUCCESSFUL) {
        std::cerr << "Failed to change screen resolution. Error code: " << result << std::endl;
        return false;
    }
    std::cout << "Resolution changed to: " << width << "x" << height << std::endl;
    return true;
}

    bool setQuietMode(bool enable) {
        HKEY hKey;
        const wchar_t* subKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\PushNotifications";

        LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, subKey, 0, KEY_SET_VALUE, &hKey);
        if (result != ERROR_SUCCESS) {
            std::cerr << "RegOpenKeyExW failed with error: " << result << "\n";
            return false;
        }

        DWORD value = enable ? 1 : 0;
        result = RegSetValueExW(hKey, L"ToastEnabled", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value));
        RegCloseKey(hKey);

        if (result != ERROR_SUCCESS) {
            std::cerr << "RegSetValueExW failed with error: " << result << "\n";
            return false;
        }

        return true;
    }

    bool setDisplayOrientation(DWORD orientation) {
        DEVMODE devMode = {};
        devMode.dmSize = sizeof(DEVMODE);
        devMode.dmFields = DM_DISPLAYORIENTATION;

        if (!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode))
            return false;

        // Перевірка, чи вже така орієнтація
        if (devMode.dmDisplayOrientation == orientation)
            return true;

        // Якщо переходимо між горизонтальним ↔ вертикальним, треба поміняти ширину і висоту
        if ((devMode.dmDisplayOrientation == DMDO_90 || devMode.dmDisplayOrientation == DMDO_270) !=
            (orientation == DMDO_90 || orientation == DMDO_270)) {
            devMode.dmFields |= DM_PELSWIDTH | DM_PELSHEIGHT;
            std::swap(devMode.dmPelsWidth, devMode.dmPelsHeight);
        }

        devMode.dmDisplayOrientation = orientation;

        LONG result = ChangeDisplaySettingsEx(NULL, &devMode, NULL, CDS_UPDATEREGISTRY | CDS_RESET, NULL);
        return (result == DISP_CHANGE_SUCCESSFUL);
    }


