#include <iostream>
#include <thread>
#include <windows.h>
#include "Server.h"

int main()
{
    std::thread udpThread(broadcastUDP);
    tcpServer();

    udpThread.join();
    return 0;
}
