#include <iostream>
#include <thread>

#include "Server.h"
#include <windows.h>

int main() {
    std::thread udpThread(broadcastUDP);
    tcpServer();

    udpThread.join();
    return 0;
}
