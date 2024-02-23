#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <cstring>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

bool running = true;
std::vector<std::pair<SOCKET, std::string>> clients;

void handleClient(SOCKET clientSocket, const std::string& name) {
    char buffer[1024];
    while (running) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cout << "Connection lost with client: " << name << std::endl;
            closesocket(clientSocket);

            // Remove disconnected client from the list
            for (auto it = clients.begin(); it != clients.end(); ++it) {
                if (it->first == clientSocket) {
                    clients.erase(it);
                    break;
                }
            }

            return;
        }
        std::string message = std::string(buffer, bytesReceived);
        std::cout << name << ": " << message << std::endl;

        // Check if client sent "exit"
        if (message == "exit") {
            std::cout << "Client " << name << " sent exit message. Disconnecting..." << std::endl;
            closesocket(clientSocket);

            // Remove client from the list
            for (auto it = clients.begin(); it != clients.end(); ++it) {
                if (it->first == clientSocket) {
                    clients.erase(it);
                    break;
                }
            }

            return;
        }

        // Send received message to all other clients
        for (const auto& otherClient : clients) {
            if (otherClient.first != clientSocket) {
                send(otherClient.first, (name + ": " + message).c_str(), message.size() + name.size() + 2, 0);
            }
        }
    }
}

int main() {
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    int wsOK = WSAStartup(ver, &wsData);
    if (wsOK != 0) {
        std::cerr << "Can't initialize Winsock! Quitting" << std::endl;
        return -1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Can't create socket! Quitting" << std::endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&hint, sizeof(hint));
    listen(serverSocket, SOMAXCONN);

    std::cout << "Server is listening for connections..." << std::endl;

    while (running) {
        sockaddr_in client;
        int clientSize = sizeof(client);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&client, &clientSize);

        char host[NI_MAXHOST];
        char service[NI_MAXSERV];
        memset(host, 0, NI_MAXHOST);
        memset(service, 0, NI_MAXSERV);
        getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0);

        char clientName[4096];
        int nameSize = recv(clientSocket, clientName, sizeof(clientName), 0);

        clientName[nameSize] = '\0'; // Add null terminator
        std::cout << "Connected: " << clientName << " on port " << service << std::endl;

        clients.push_back({ clientSocket, std::string(clientName) });

        std::thread t(handleClient, clientSocket, std::string(clientName));
        t.detach();
    }


    for (const auto& client : clients) {
        closesocket(client.first);
    }
    closesocket(serverSocket);

    WSACleanup();

    return 0;
}