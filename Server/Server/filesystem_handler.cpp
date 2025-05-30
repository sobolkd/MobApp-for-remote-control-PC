#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <locale>
#include <codecvt>

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

std::wstring stringToWstring2(const std::string& str)
{
    if (str.empty()) return std::wstring();

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::wstring normalizePathSeparators(const std::wstring& path) {
    std::wstring normalized = path;
    std::replace(normalized.begin(), normalized.end(), L'/', L'\\');
    return normalized;
}

void handleDeleteFile(const std::string& command, SOCKET clientSock)
{
    std::string filePath = command.substr(7);
    std::wstring wFilePath = normalizePathSeparators(stringToWstring2(filePath));

    if (DeleteFileW(wFilePath.c_str()))
    {
        std::string response = "Success!";
        send(clientSock, response.c_str(), (int)response.size(), 0);
    }
    else
    {
        std::string response = "Failed deleting file";
        send(clientSock, response.c_str(), (int)response.size(), 0);
    }
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

bool sendFileToClient(const std::wstring& filePath, SOCKET clientSocket)
{
    HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        std::wcerr << L"[ERROR] Cannot open file: " << filePath << L"\n";
        return false;
    }

    LARGE_INTEGER fileSizeLI;
    if (!GetFileSizeEx(hFile, &fileSizeLI))
    {
        std::wcerr << L"[ERROR] Cannot get file size: " << filePath << L"\n";
        CloseHandle(hFile);
        return false;
    }

    if (fileSizeLI.QuadPart > UINT32_MAX)
    {
        std::wcerr << L"[ERROR] File too large: " << filePath << L"\n";
        CloseHandle(hFile);
        return false;
    }

    uint32_t fileSize = static_cast<uint32_t>(fileSizeLI.QuadPart);
    std::vector<char> buffer(fileSize);

    DWORD bytesRead = 0;
    if (!ReadFile(hFile, buffer.data(), fileSize, &bytesRead, nullptr) || bytesRead != fileSize)
    {
        std::wcerr << L"[ERROR] Cannot read file: " << filePath << L"\n";
        CloseHandle(hFile);
        return false;
    }

    CloseHandle(hFile);

    if (send(clientSocket, reinterpret_cast<const char*>(&fileSize), sizeof(fileSize), 0) <= 0)
    {
        std::cerr << "[ERROR] Error sending size\n";
        return false;
    }

    size_t totalSent = 0;
    while (totalSent < buffer.size())
    {
        int sent = send(clientSocket, buffer.data() + totalSent, buffer.size() - totalSent, 0);
        if (sent <= 0)
        {
            std::cerr << "[ERROR] Error sending file data\n";
            return false;
        }
        totalSent += sent;
    }

    std::wcout << L"[LOG] File has been fully sent: " << filePath << L"\n";
    return true;
}

void handleCopyFile(const std::string& command, SOCKET clientSock)
{
    try
    {
        size_t firstUnderscore = command.find('_');
        size_t secondUnderscore = command.find('_', firstUnderscore + 1);

        if (firstUnderscore == std::string::npos || secondUnderscore == std::string::npos)
        {
            std::string response = "Invalid COPY command format.";
            send(clientSock, response.c_str(), (int)response.size(), 0);
            return;
        }

        std::string sourcePath = command.substr(firstUnderscore + 1, secondUnderscore - firstUnderscore - 1);
        std::string destPath = command.substr(secondUnderscore + 1);

        std::wstring wSourcePath = normalizePathSeparators(stringToWstring2(sourcePath));
        std::wstring wDestPath = normalizePathSeparators(stringToWstring2(destPath));

        std::wcout << L"[LOG] Copying file from: " << wSourcePath << L" to " << wDestPath << std::endl;

        if (fs::is_directory(wDestPath)) {
            wDestPath += L"\\" + fs::path(wSourcePath).filename().wstring();
        }

        if (CopyFileW(wSourcePath.c_str(), wDestPath.c_str(), FALSE))
        {
            std::string response = "Copy successful!";
            send(clientSock, response.c_str(), (int)response.size(), 0);
        }
        else
        {
            std::wstring errorMsg = L"[ERROR] Failed to copy file. Error code: " + std::to_wstring(GetLastError());
            std::wcerr << errorMsg << std::endl;

            std::string response = "Failed to copy file.";
            send(clientSock, response.c_str(), (int)response.size(), 0);
        }
    }
    catch (const std::exception& e)
    {
        std::wcerr << L"[EXCEPTION] " << e.what() << std::endl;
        std::string response = "Exception during file copy.";
        send(clientSock, response.c_str(), (int)response.size(), 0);
    }
}

void handleMoveFile(const std::string& command, SOCKET clientSock)
{
    try
    {
        size_t firstUnderscore = command.find('_');
        size_t secondUnderscore = command.find('_', firstUnderscore + 1);

        if (firstUnderscore == std::string::npos || secondUnderscore == std::string::npos)
        {
            std::string response = "Invalid CUT command format.";
            send(clientSock, response.c_str(), (int)response.size(), 0);
            return;
        }

        std::string sourcePath = command.substr(firstUnderscore + 1, secondUnderscore - firstUnderscore - 1);
        std::string destPath = command.substr(secondUnderscore + 1);

        std::wstring wSourcePath = normalizePathSeparators(stringToWstring2(sourcePath));
        std::wstring wDestPath = normalizePathSeparators(stringToWstring2(destPath));

        std::wcout << L"[LOG] Moving file from: " << wSourcePath << L" to " << wDestPath << std::endl;

        if (fs::is_directory(wDestPath)) {
            wDestPath += L"\\" + fs::path(wSourcePath).filename().wstring();
        }

        if (MoveFileExW(wSourcePath.c_str(), wDestPath.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING))
        {
            std::string response = "Cut successful!";
            send(clientSock, response.c_str(), (int)response.size(), 0);
        }
        else
        {
            std::wstring errorMsg = L"[ERROR] Failed to cut file. Error code: " + std::to_wstring(GetLastError());
            std::wcerr << errorMsg << std::endl;

            std::string response = "Failed to cut file.";
            send(clientSock, response.c_str(), (int)response.size(), 0);
        }
    }
    catch (const std::exception& e)
    {
        std::wcerr << L"[EXCEPTION] " << e.what() << std::endl;
        std::string response = "Exception during file cut.";
        send(clientSock, response.c_str(), (int)response.size(), 0);
    }
}

