#include "Server.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wininet.h>
#include "MouseController.h"
#include <stdio.h>


#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wininet.lib")

#define _CRT_SECURE_NO_WARNINGS
#define UDP_PORT 8888
#define TCP_PORT 9999
#define BROADCAST_IP "255.255.255.255"
#define KEYWORD "HELLO_SERVER"

bool stopBroadcast = false;

// Отримання локальної IP-адреси
std::string getLocalIPAddress() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        return "0.0.0.0";
    }

    struct addrinfo hints {}, * info, * p;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(hostname, nullptr, &hints, &info) != 0) {
        return "0.0.0.0";
    }

    std::string localIP = "0.0.0.0";
    char ipStr[INET_ADDRSTRLEN];

    for (p = info; p != nullptr; p = p->ai_next) {
        struct sockaddr_in* addr = (struct sockaddr_in*)p->ai_addr;
        if (InetNtopA(AF_INET, &addr->sin_addr, ipStr, INET_ADDRSTRLEN)) {
            localIP = ipStr;
            if (localIP != "127.0.0.1") {
                break;
            }
        }
    }

    freeaddrinfo(info);
    return localIP;
}

// Отримання глобальної IP-адреси через веб-сервіс
std::string getGlobalIPAddress() {
    HINTERNET hInternet, hConnect;
    DWORD bytesRead;
    char buffer[128];

    hInternet = InternetOpen(L"GET Global IP", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        return "Error retrieving global IP";
    }

    hConnect = InternetOpenUrl(hInternet, L"https://api.ipify.org", NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        InternetCloseHandle(hInternet);
        return "Error retrieving global IP";
    }

    InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead);
    buffer[bytesRead] = '\0';

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return std::string(buffer);
}

// Функція трансляції UDP-повідомлень
void broadcastUDP() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Cannot create UDP socket\n";
        return;
    }

    BOOL broadcastEnable = TRUE;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&broadcastEnable, sizeof(broadcastEnable));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDP_PORT);
    InetPtonA(AF_INET, BROADCAST_IP, &addr.sin_addr);

    while (!stopBroadcast) {
        sendto(sock, KEYWORD, strlen(KEYWORD), 0, (sockaddr*)&addr, sizeof(addr));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    closesocket(sock);
    WSACleanup();
}

// Обробка підключення клієнта
void handleClient(SOCKET clientSock) {
    char buffer[256];
    int bytesReceived = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::string command(buffer);

        if (command.rfind("MOVE", 0) == 0) {
            int x, y;
            sscanf_s(command.c_str(), "MOVE %d %d", &x, &y);
            moveCursor(x, y);
        }
        else if (command == "CLICK LEFT") {
            clickMouse(true);
        }
        else if (command == "CLICK RIGHT") {
            clickMouse(false);
        }
    }
    closesocket(clientSock);
}

// TCP-сервер, що працює безперервно
void tcpServer() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock == INVALID_SOCKET) {
        std::cerr << "Cannot create TCP socket\n";
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TCP_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error binding TCP socket\n";
        closesocket(serverSock);
        WSACleanup();
        return;
    }

    listen(serverSock, SOMAXCONN);
    std::cout << "Server is listening on port " << TCP_PORT << "...\n";

    while (true) {
        sockaddr_in clientAddr{};
        int clientLen = sizeof(clientAddr);
        SOCKET clientSock = accept(serverSock, (sockaddr*)&clientAddr, &clientLen);
        if (clientSock == INVALID_SOCKET) {
            std::cerr << "Error while accepting connection\n";
            continue;
        }

        if (!stopBroadcast) {
            stopBroadcast = true;
        }

        // Створюємо потік для обробки клієнта
        std::thread clientThread(handleClient, clientSock);
        clientThread.detach();
    }

    closesocket(serverSock);
    WSACleanup();
}
