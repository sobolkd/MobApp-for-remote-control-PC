#include <iostream>
#include <thread>
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include "Server.h"
#include "TrayIcon.h"
#include "sqlite3.h"
#include "sqlite_helper.h"

int main()
{
    // hide program 
    InitTrayIcon();

    std::cout << "Starting server..." << std::endl;
    // start server
    SetConsoleOutputCP(CP_UTF8);
    _setmode(_fileno(stdout), _O_TEXT);

    std::thread udpThread(broadcastUDP);
    std::thread tcpThread(tcpServer);

    // check db
    if (!init_database()) {
        printf("Database init failed\n");
        return 1;
    }

    if (!insert_user("admin2", "secret1234")) {
        printf("Insert failed\n");
    }

    print_all_users();

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
