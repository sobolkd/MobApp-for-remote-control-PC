#pragma once
#include <string>

bool addToAutostart(const std::wstring& appName, const std::wstring& appPath);
bool removeFromAutostart(const std::wstring& appName);
