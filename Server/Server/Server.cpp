// libs
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <sstream>
#include <vector>
// headers
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wininet.h>
#include "MouseController.h"
#include "Display.h"
#include "KeyboardController.h"
#include "ClipboardController.h"
#include "MediaController.h"
#include "SystemFunctions.h"

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

std::wstring stringToWstring(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
    return wstr;
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

        /* here all comands from app*/

        // move cursor
        if (command.rfind("MOVE", 0) == 0) {
            int x, y;
            sscanf_s(command.c_str(), "MOVE %d %d", &x, &y);
            moveCursor(x, y);
        }
        // left click
        else if (command == "CLICK LEFT") {
            clickMouse(true);
        }
        //right click
        else if (command == "CLICK RIGHT") {
            clickMouse(false);
        }
        // change brightness
        else if (command.rfind("BRIGHTNESS", 0) == 0) {
            int level;
            sscanf_s(command.c_str(), "BRIGHTNESS %d", &level);
            if (level >= 0 && level <= 100) {
                setMonitorBrightness(level);
            }   
        }
        // call on-screen keyboard
        else if (command == "OPEN_KEYBOARD") {
            openOnScreenKeyboard();
        }
        // scroll
        else if (command.rfind("SCROLL", 0) == 0) {
            int amount;
            sscanf_s(command.c_str(), "SCROLL %d", &amount);
            scrollMouse(amount);
        }
        // copy
        else if (command.rfind("CTRL C", 0) == 0) {
            pressCtrlC();
        }
        // paste
        else if (command.rfind("CTRL V", 0) == 0) {
            pressCtrlV();
        }
        // Pause/Unpause music
        else if (command == "MEDIA PLAYPAUSE") {
            PlayPause();
        }
        // next media-file
        else if (command == "MEDIA NEXT") {
            NextTrack();

        }
        // previous media-file
        else if (command == "MEDIA PREV") {
            PrevTrack();
        }
        // volume = 0
        else if (command == "VOLUME MUTE") {
            Mute();
        }
        // change volume
        else if (command.rfind("VOLUME", 0) == 0) {
            int volume;
            sscanf_s(command.c_str(), "VOLUME %d", &volume);
            ChangeVolume(volume);
        }
        // get name of media-file
        else if (command == "GET NOWPLAYING") {
            std::string nowPlaying = GetNowPlayingInfo();
            send(clientSock, nowPlaying.c_str(), nowPlaying.length(), 0);
        }
        // get volume level
        else if (command == "GET VOLUME") {
            int volume = GetCurrentVolume();
            std::string volumeStr = std::to_string(volume);
            send(clientSock, volumeStr.c_str(), volumeStr.length(), 0);
        }
        //get audio-outputs
        else if (command.rfind("GET_AUDIO_OUTPUTS", 0) == 0)
        {
            std::vector<std::wstring> audioOutputs = GetAudioOutputs();
            std::wstring outputStr = L"Available Audio Outputs:\n";
            for (const auto& output : audioOutputs) {
                outputStr += output + L"\n";
            }

            std::string outputStrUtf8(outputStr.begin(), outputStr.end());
            std::cout << outputStrUtf8 << std::endl;
            send(clientSock, outputStrUtf8.c_str(), outputStrUtf8.length(), 0);
        }
        // change audio output
        else if (command.rfind("CHANGE_AUDIO_OUTPUTS", 0) == 0)
        {
            std::wstring deviceId = std::wstring(command.begin() + std::wstring(L"CHANGE_AUDIO_OUTPUTS ").length(), command.end());

            bool success = SetAudioOutput(deviceId);
            if (success)
                std::cout << "Audio output changed successfully\n";
            else
                std::cout << "Failed to change audio output\n";
        }
        // get brightness level
        else if (command == "GET_BRIGHTNESS") {
            int brightness = getMonitorBrightness();
            std::string response = (brightness >= 0) ? std::to_string(brightness) : "ERROR";
            send(clientSock, response.c_str(), response.size(), 0);
        }
        // set clipboard
        else if (command.compare(0, 14, "SET_CLIPBOARD ") == 0) {
            std::wstring text = stringToWstring(command.substr(14));

            bool success = setClipboardText(text);
            std::string response = success ? "OK" : "ERROR";
            send(clientSock, response.c_str(), response.size(), 0);
        }
        // set resolution
        else if (command.compare(0, 15, "SET_RESOLUTION ") == 0) {
            std::istringstream iss(command.substr(15));
            int width, height;
            if (iss >> width >> height) {
                bool success = setScreenResolution(width, height);
            }
            else {
                std::cout << "Invalid Format" << std::endl;
            }
        }
        // quiet mode
        else if (command.compare(0, 15, "QUIET_MODE_ON") == 0) {
            setQuietMode(true);
            std::cout << "Quiet mode ON." << std::endl;
        }
        else if (command.compare(0, 16, "QUIET_MODE_OFF") == 0) {
            setQuietMode(false);
            std::cout << "Quiet mode OFF." << std::endl;
        }
        // change orientation
        else if (command.rfind("CHANGE_ORIENTATION", 0) == 0) {
            int angle = 0;
            if (sscanf_s(command.c_str(), "CHANGE_ORIENTATION %d", &angle) == 1) {
                int orientation = DMDO_DEFAULT;
                if (angle == 90) orientation = DMDO_90;
                else if (angle == 180) orientation = DMDO_180;
                else if (angle == 270) orientation = DMDO_270;
                else orientation = DMDO_DEFAULT;

                bool result = setDisplayOrientation(orientation);
                std::cout << (result ? "Orientation changed." : "Orientation changing failed") << std::endl;
            }
            else {
                std::cout << "Invalid orientation command." << std::endl;
            }
        }
        // sleep mode
        else if (command.rfind("SLEEP_MODE_ON", 0) == 0)
        {
            setMonitorSleep();
            std::cout << "Sleep mode ON." << std::endl;
        }
        else if (command.rfind("SLEEP_MODE_OFF", 0) == 0)
        {
            wakeUpMonitor();
            std::cout << "Sleep mode OFF." << std::endl;
        }
        // restart PC
        else if (command.rfind("RESTART_PC", 0) == 0)
        {
            bool result = restartComputer();
            if (result)
            {
                std::cout << "Restarting PC..." << std::endl;
            }
            else
            {
                std::cout << "Error with restarting PC." << std::endl;
            }
        }
        // shut down PC
        else if (command.rfind("SHUT_DOWN_PC", 0) == 0)
        {
            bool result = shutdownComputer();
            if (result)
            {
                std::cout << "Shutting down PC..." << std::endl;
            }
            else
            {
                std::cout << "Error with shutting down PC." << std::endl;
            }
        }
        // Lock Screen
        else if (command.rfind("LOCK_SCREEN_PC", 0) == 0)
        {
            bool result = lockWorkstation();
            if (result)
            {
                std::cout << "Lock Screen." << std::endl;
            }
            else
            {
                std::cout << "Error with locking screen." << std::endl;
            }
        }
        // End User Session
        else if (command.rfind("END_SESSION", 0) == 0)
        {
            bool result = logoffUser();
            if (result)
            {
                std::cout << "Log off user." << std::endl;
            }
            else
            {
                std::cout << "Error with logging off user." << std::endl;
            }
        }
        // Call Task Manager
        else if (command.rfind("CALL_TASK_MANAGER", 0) == 0)
        {
            bool result = openTaskManager();
            if (result)
            {
                std::cout << "Open Task Manager." << std::endl;
            }
            else
            {
                std::cout << "Error with opening task manager." << std::endl;
            }
        }
        



    }
    closesocket(clientSock);
}

void sendResult(bool success, SOCKET clientSock) {
    std::string response = success ? "OK" : "ERROR";
    send(clientSock, response.c_str(), response.size(), 0);
}


void tcpServer() {

    std::cout << R"(  
  _________     ___.          .__   
 /   _____/ ____\_ |__   ____ |  |  
 \_____  \ /  _ \| __ \ /  _ \|  |  
 /        (  <_> ) \_\ (  <_> )  |__
/_______  /\____/|___  /\____/|____/
        \/           \/             
)" << std::endl;
std::cout << "Server started..." << std::endl;

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

