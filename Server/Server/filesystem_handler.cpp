#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

struct FileEntry {
    std::wstring name;
    bool is_directory;
};

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

std::string wstring_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &strTo[0], size_needed, nullptr, nullptr);
    return strTo;
}

std::vector<FileEntry> list_directory_contents(const std::wstring& path) {
    std::vector<FileEntry> entries;
    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            entries.push_back({
                entry.path().filename(),
                entry.is_directory()
                });
        }
    }
    catch (const std::exception& e) {
        std::wcerr << L"Directory access error: " << e.what() << L" Path: " << path << std::endl;
    }
    return entries;
}

std::string get_driver_list_response() {
    std::vector<std::wstring> drives = list_logical_drives();
    std::ostringstream oss;

    for (const auto& drive : drives) {
        oss << "[DRIVE] " << wstring_to_utf8(drive) << "\n";
    }

    return oss.str();
}

std::string get_directory_list_response(const std::wstring& path) {
    auto entries = list_directory_contents(path);
    std::ostringstream oss;

    for (const auto& entry : entries) {
        std::string utf8_name = wstring_to_utf8(entry.name);
        if (entry.is_directory)
            oss << "[DIR] " << utf8_name << "\n";
        else
            oss << "[FILE] " << utf8_name << "\n";
    }

    return oss.str();
}
