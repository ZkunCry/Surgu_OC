#include <iostream>
#include <string>
#include <cstring>
#include <WS2tcpip.h>
#include <thread>
#include <mutex>
#pragma comment(lib, "ws2_32.lib")
std::mutex mut;
bool isDisconnectOrErrors = false;

void receiveMessages(SOCKET sock) {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesReceived == 0) {
            isDisconnectOrErrors = true;
            std::cout << "Server closed the connection. Quitting..." << std::endl;
            break;
        }
        else if (bytesReceived == SOCKET_ERROR) {
            isDisconnectOrErrors = true;
            std::cerr << "Error in receiving data from server. Quitting..." << std::endl;
            break;
        }
    }
}
int main() {
    //Инициализация WinSock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    int wsOK = WSAStartup(ver, &wsData);
    if (wsOK != 0) {
        std::cerr << "Can't initialize Winsock! Quitting" << std::endl;
        return -1;
    }
    //Создание сокета с использованием протокола TCP
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Can't create socket! Quitting" << std::endl;
        WSACleanup();
        return -1;
    }
    //Установка соединения с сервером
    /*Создается структура sockaddr_in, которая содержит информацию об адресе и порте сервера.
        inet_pton() конвертирует строку "127.0.0.1" в формат сетевого адреса и сохраняет его в структуре.
        Далее выполняется вызов connect() для установки соединения с сервером.*/
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr);

    int result = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (result == SOCKET_ERROR) {
        std::cerr << "Can't connect to the server! Quitting" << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }
    //Отправка имени клиента на сервер
    std::string name;
    std::cout << "Enter your name: ";
    std::getline(std::cin, name);
    send(sock, name.c_str(), name.size(), 0);
    std::string message;
    /*Запуск обработчика событий.
       Выставляет флаг isDisconnectedOrErrors в true, если произошла ошибка со стороны сервера
       Запускается в отдельном потоке для того, чтобы не блокировался основной поток.
    */
    std::thread receiver(receiveMessages, sock);
    char buffer[1024];
    while (message !="exit" && !isDisconnectOrErrors) {
        if (isDisconnectOrErrors)
            break;
        std::cout << "Enter your message: ";
        std::getline(std::cin, message);
        send(sock, message.c_str(), message.size(), 0);
  
    }
    //Ожидание
    receiver.join();
    std::cout << "Disconnecting" << std::endl;
    closesocket(sock);
    WSACleanup();

    return 0;
}