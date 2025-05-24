#include "upnp_server.h"
#include <winsock2.h>

extern "C" {
#include <miniupnpc.h>
#include <upnpcommands.h>
#include <upnperrors.h>
}

#include <iostream>
#include <fstream>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

static bool open_upnp_port(int port) {
    UPNPDev* devlist = upnpDiscover(2000, nullptr, nullptr, 0, 0, 2, nullptr);
    if (!devlist) {
        std::cerr << "UPnP: No devices found\n";
        return false;
    }

    UPNPUrls urls{};
    IGDdatas data{};
    char lanaddr[64]{};
    char wanaddr[64]{};

    if (UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr), wanaddr, sizeof(wanaddr)) != 1) {
        std::cerr << "UPnP: No valid IGD found\n";
        freeUPNPDevlist(devlist);
        return false;
    }

    char port_str[6];
    sprintf_s(port_str, sizeof(port_str), "%d", port);

    int r = UPNP_AddPortMapping(
        urls.controlURL,
        data.first.servicetype,
        port_str, port_str,
        lanaddr,
        "C++ UPnP Server",
        "TCP",
        nullptr, nullptr
    );

    freeUPNPDevlist(devlist);
    FreeUPNPUrls(&urls);

    if (r != UPNPCOMMAND_SUCCESS) {
        std::cerr << "UPnP: Port mapping failed: " << strupnperror(r) << "\n";
        return false;
    }

    std::cout << "UPnP: Port " << port << " mapped successfully.\n";
    return true;
}

static bool send_file(SOCKET client_socket, const char* file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << file_path << "\n";
        return false;
    }

    file.seekg(0, std::ios::end);
    size_t size = static_cast<size_t>(file.tellg());
    file.seekg(0);

    std::vector<char> buffer(size);
    file.read(buffer.data(), size);

    int sent = send(client_socket, buffer.data(), static_cast<int>(buffer.size()), 0);
    if (sent == SOCKET_ERROR) {
        std::cerr << "Send failed\n";
        return false;
    }

    std::cout << "Sent " << sent << " bytes to client\n";
    return true;
}

bool start_server_with_upnp(int port, const char* image_path) {
    if (!open_upnp_port(port)) return false;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return false;
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return false;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(server_socket, (sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "Bind failed\n";
        closesocket(server_socket);
        WSACleanup();
        return false;
    }

    listen(server_socket, 1);
    std::cout << "Waiting for client on port " << port << "...\n";

    SOCKET client_socket = accept(server_socket, nullptr, nullptr);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Accept failed\n";
        closesocket(server_socket);
        WSACleanup();
        return false;
    }

    std::cout << "Client connected!\n";
    bool success = send_file(client_socket, image_path);

    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();

    return success;
}
