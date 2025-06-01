#include <iostream>
#include <thread>
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include "Server.h"
#include "TrayIcon.h"
#include "sqlite3.h"
#include "sqlite_helper.h"

// TODO: add server to the autolaunch
// TODO: fix bug with "Back" button about path
// TODO: add check for symbols
// TODO: change UI for System

int main()
{
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
