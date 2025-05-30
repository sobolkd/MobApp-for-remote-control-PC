#include <iostream>
#include <thread>
#include <windows.h>
#include <fcntl.h>
#include <io.h>

#include "Server.h"
#include "TrayIcon.h"

int main()
{
    InitTrayIcon();

    std::cout << "Starting server..." << std::endl;
    SetConsoleOutputCP(CP_UTF8);
    _setmode(_fileno(stdout), _O_TEXT);

    std::thread udpThread(broadcastUDP);
    std::thread tcpThread(tcpServer);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    udpThread.join();
    tcpThread.join();

    ShutdownTrayIcon();

    return 0;
}
