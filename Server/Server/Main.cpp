#include <iostream>
#include <thread>
#include <windows.h>
#include "Server.h"
#include <fcntl.h>
#include <io.h>
int main()
{
    SetConsoleOutputCP(CP_UTF8);
    _setmode(_fileno(stdout), _O_TEXT);
    std::thread udpThread(broadcastUDP);
    tcpServer();

    udpThread.join();
    return 0;
}
