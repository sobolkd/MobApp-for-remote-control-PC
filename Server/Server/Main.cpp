#include <iostream>
#include <thread>
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include "Server.h"
#include "TrayIcon.h"
#include "sqlite3.h"
#include "sqlite_helper.h"
#include "autostart.h"
#include <filesystem>

// TODO: add server to the autolaunch
// TODO: fix bug with "Back" button about path

int main()
{
    try {
    // Add program to autolaunch
    std::wstring appName = L"RemoteServer";
    std::wstring appPath = std::filesystem::current_path().wstring() + L"\\Server.exe";

    std::wstring appDir = std::filesystem::current_path().wstring();
    SetCurrentDirectoryW(appDir.c_str());

    if (!addToAutostart(appName, appPath)) {
        std::wcout << L"Failed to add to autostart." << std::endl;
    }
    else {
        std::wcout << L"Successfully added to autostart!" << std::endl;
    }

    // hide program 
    InitTrayIcon();

    std::cout << "Starting server..." << std::endl;

    SetConsoleOutputCP(CP_UTF8);
    _setmode(_fileno(stdout), _O_TEXT);

    std::thread udpThread(broadcastUDP);
    std::thread tcpThread(tcpServer);

    // check db
    if (!init_database()) {
        printf("Database init failed\n");
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    // start server
    udpThread.join();
    tcpThread.join();

    ShutdownTrayIcon();

    return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
        MessageBoxA(NULL, e.what(), "Unhandled std::exception", MB_OK | MB_ICONERROR);
        return -1;
    }
    catch (...) {
        std::cerr << "Unknown exception occurred" << std::endl;
        MessageBoxA(NULL, "Unknown exception", "Fatal Error", MB_OK | MB_ICONERROR);
        return -2;
    }
}
