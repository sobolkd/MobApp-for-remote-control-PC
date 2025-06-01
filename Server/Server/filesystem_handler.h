#pragma once

#include <string>
#include <vector>
#include <fstream>

struct FileEntry {
    std::wstring name;
    bool is_directory;
};

std::vector<std::wstring> list_logical_drives();
std::string get_driver_list_response();

std::vector<FileEntry> list_directory_contents(const std::wstring& path);
std::string get_directory_list_response(const std::wstring& path);
bool sendFileToClient(const std::wstring& filePath, SOCKET clientSocket);
void handleDeleteFile(const std::string& command, SOCKET clientSock);
void handleCopyFile(const std::string& command, SOCKET clientSock);
void handleMoveFile(const std::string& command, SOCKET clientSock);
std::wstring stringToWstring2(const std::string& str);
std::wstring normalizePathSeparators(const std::wstring& path);