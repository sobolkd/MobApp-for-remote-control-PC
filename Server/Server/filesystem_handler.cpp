#include "filesystem_handler.h"

#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <locale>
#include <codecvt>
#include <iostream>

namespace fs = std::filesystem;

std::vector<std::wstring> list_logical_drives() {
    std::vector<std::wstring> drives;
    DWORD mask = GetLogicalDrives();
    for (int i = 0; i < 26; ++i) {
        if (mask & (1 << i)) {
            wchar_t letter = L'A' + i;
            drives.push_back(std::wstring(1, letter) + L":\\");
        }
    }
    return drives;
}

std::string get_driver_list_response() {
    std::vector<std::wstring> drives = list_logical_drives();
    std::ostringstream oss;

    for (const auto& drive : drives) {
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, drive.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (size_needed > 0) {
            std::string utf8_drive(size_needed - 1, 0);
            WideCharToMultiByte(CP_UTF8, 0, drive.c_str(), -1, utf8_drive.data(), size_needed, nullptr, nullptr);

            oss << "[DRIVE] " << utf8_drive << "\n";
        }
    }

    return oss.str();
}

std::vector<FileEntry> list_directory_contents(const std::wstring& path) {
    std::vector<FileEntry> entries;
    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            entries.push_back({
                entry.path().filename().wstring(),
                entry.is_directory()
                });
        }
    }
    catch (...) {
        std::cerr << "Ignoring root issues." << GetLastError() << std::endl;
    }
    return entries;
}

std::string get_directory_list_response(const std::wstring& path) {
    auto entries = list_directory_contents(path);
    std::ostringstream oss;

    for (const auto& entry : entries) {
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, entry.name.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (size_needed > 0) {
            std::string utf8_name(size_needed - 1, 0);
            WideCharToMultiByte(CP_UTF8, 0, entry.name.c_str(), -1, utf8_name.data(), size_needed, nullptr, nullptr);

            if (entry.is_directory)
                oss << "[DIR] " << utf8_name << "\n";
            else
                oss << "[FILE] " << utf8_name << "\n";
        }
    }

    return oss.str();
}
