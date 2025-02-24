// libs
#include <iostream>
#include <thread>
#include <chrono>

// headers
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wininet.h>
#include "MouseController.h"
#include "Display.h"
#include "KeyboardController.h"
#include "ClipboardController.h"
#include "MediaController.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wininet.lib")

#define UDP_PORT 8888
#define TCP_PORT 9999
#define BROADCAST_IP "255.255.255.255"
#define KEYWORD "HELLO_SERVER"

bool stopBroadcast = false;
bool isSended = false;

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

void broadcastUDP() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) return;

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

std::string getGlobalIPAddress() {
    HINTERNET hInternet, hConnect;
    DWORD bytesRead;
    char buffer[128] = { 0 };
    ;
    hInternet = InternetOpen(L"GET Global IP", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        return "Error retrieving global IP";
    }

    hConnect = InternetOpenUrl(hInternet, L"https://api.ipify.org", NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        InternetCloseHandle(hInternet);
        return "Error retrieving global IP";
    }

    if (!InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead) || bytesRead == 0) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return "Error reading IP";
    }

    buffer[bytesRead] = '\0';

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return std::string(buffer);
}

void handleClient(SOCKET clientSock) {
    
    if (isSended == false)
    {
        std::string globalIp = getGlobalIPAddress();
        std::cout << "Sending global IP: " << globalIp << std::endl;
        send(clientSock, globalIp.c_str(), globalIp.length(), 0);
        isSended = true;
    }
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
        else if (command.rfind("BRIGHTNESS", 0) == 0) {
            int level;
            sscanf_s(command.c_str(), "BRIGHTNESS %d", &level);
            if (level >= 0 && level <= 100) {
                setMonitorBrightness(level);
            }   
        }
        else if (command == "OPEN_KEYBOARD") {
            openOnScreenKeyboard();
        }
        else if (command.rfind("SCROLL", 0) == 0) {
            int amount;
            sscanf_s(command.c_str(), "SCROLL %d", &amount);
            scrollMouse(amount);
        }
        else if (command.rfind("CTRL C", 0) == 0) {
            pressCtrlC();
        }
        else if (command.rfind("CTRL V", 0) == 0) {
            pressCtrlV();
        }
        else if (command == "MEDIA PLAYPAUSE") {
            PlayPause();
        }
        else if (command == "MEDIA NEXT") {
            NextTrack();
        }
        else if (command == "MEDIA PREV") {
            PrevTrack();
        }
        else if (command == "VOLUME MUTE") {
            Mute();
        }
        else if (command.rfind("VOLUME", 0) == 0) {
            int volume;
            sscanf_s(command.c_str(), "VOLUME %d", &volume);
            ChangeVolume(volume);
        }
        else if (command == "GET NOWPLAYING") {
            std::string nowPlaying = GetNowPlayingInfo();
            send(clientSock, nowPlaying.c_str(), nowPlaying.length(), 0);
        }
        else if (command == "GET VOLUME") {
            int volume = GetCurrentVolume();
            std::string volumeStr = std::to_string(volume);
            send(clientSock, volumeStr.c_str(), volumeStr.length(), 0);
        }

    }
    closesocket(clientSock);
}

void tcpServer() {
    std::cout << "Server started...\n";
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock == INVALID_SOCKET) return;

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TCP_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(serverSock);
        WSACleanup();
        return;
    }

    if (listen(serverSock, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(serverSock);
        WSACleanup();
        return;
    }

    while (true) {
        sockaddr_in clientAddr{};
        int clientLen = sizeof(clientAddr);
        SOCKET clientSock = accept(serverSock, (sockaddr*)&clientAddr, &clientLen);
        if (clientSock == INVALID_SOCKET) continue;

        if (!stopBroadcast) stopBroadcast = true;

        std::thread clientThread(handleClient, clientSock);
        clientThread.detach();
    }

    closesocket(serverSock);
    WSACleanup();
}

