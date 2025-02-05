#ifndef SERVER_H
#define SERVER_H

#include <string>

void broadcastUDP();
void tcpServer();
std::string getLocalIPAddress();
std::string getGlobalIPAddress();

#endif
