#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <cstring>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

bool running = true;
std::vector<std::pair<SOCKET, std::string>> clients;
//В качестве параметра принимает сокет клиента и его имя
void handleClient(SOCKET clientSocket, const std::string& name) {
    char buffer[1024]; //Буфер сообщений
    while (running) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0); //Возвращает число меньше 0 если случились ошибки
        if (bytesReceived <= 0) {
            std::cout << "Connection lost with client: " << name << std::endl;
            closesocket(clientSocket); //Закрытие текущего соединения с клиентом

            // Удаление клиента из списка если соединение прервано
            for (auto it = clients.begin(); it != clients.end(); ++it) {
                if (it->first == clientSocket) {
                    clients.erase(it);
                    break;
                }
            }

            return;
        }
        //Конвертация в тип данных message
        std::string message = std::string(buffer, bytesReceived);
        //Вывод на экран текущего пользователя который прислал сообщение
        std::cout << name << ": " << message << std::endl;

        // Проверка отправил ли в сообщении клиент exit
        if (message == "exit") {
            std::cout << "Client " << name << " sent exit message. Disconnecting..." << std::endl;
            closesocket(clientSocket); //Закрытие сокета

            // И удаление его из списка пользователей
            for (auto it = clients.begin(); it != clients.end(); ++it) {
                if (it->first == clientSocket) {
                    clients.erase(it);
                    break;
                }
            }

            return;
        }

        // Отправка сообщения всем клиентам.
        for (const auto& otherClient : clients) {
            if (otherClient.first != clientSocket) {
                send(otherClient.first, (name + ": " + message).c_str(), message.size() + name.size() + 2, 0);
            }
        }
    }
}

int main() {
    //////////////////////////////
    /*
    Инициализация WinSock
        WSADATA - это структура, используемая для хранения информации о реализации Winsock. 
        MAKEWORD(2, 2) создает версию Winsock 2.2. 
        Функция WSAStartup() инициализирует библиотеку, и переменная wsOK содержит результат этой операции.
    */
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    int wsOK = WSAStartup(ver, &wsData);
    if (wsOK != 0) {
        std::cerr << "Can't initialize Winsock! Quitting" << std::endl;
        return -1;
    }
    ///////////////////////////////////////////
    /*Создание серверного сокета
    Эта строка создает сокет сервера. AF_INET указывает на использование семейства протоколов TCP/IP, 
    SOCK_STREAM указывает на то, что используется протокол TCP для потоковой передачи данных.
    */
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Can't create socket! Quitting" << std::endl;
        WSACleanup();
        return -1;
    }
    ////////////////////////
    /*Привязка серверного сокета к адресу и порту
    Здесь устанавливаются параметры адреса и порта для сервера. 
    INADDR_ANY позволяет серверу принимать подключения на любом доступном сетевом интерфейсе.
    */
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&hint, sizeof(hint));
    ///////////////////////////
    /*Ожидание подключений
    SOMAXCONN - это максимальное количество ожидающих соединений, которые могут быть установлены для данного сокета
    */
    listen(serverSocket, SOMAXCONN);

    std::cout << "Server is listening for connections..." << std::endl;
    /*Цикл приема сообщений*/
    while (running) {
        sockaddr_in client;
        int clientSize = sizeof(client);
        //Ожидание и прием нового подключения
        /*Функция accept блокирует выполнение программы до тех пор, пока не будет получено новое подключение. 
        Когда подключение установлено, возвращается сокет для взаимодействия с клиентом.*/
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&client, &clientSize);

        char host[NI_MAXHOST]; //Массивы символов для хранения имени хоста
        char service[NI_MAXSERV]; //
        memset(host, 0, NI_MAXHOST); //Заполнение массивов
        memset(service, 0, NI_MAXSERV); //Заполнение массивов
        //Функция для получения имени хоста и сервиса для заданного адреса сокета
        getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0);
        
        char clientName[4096];
        //Прием данных от клиента
        int nameSize = recv(clientSocket, clientName, sizeof(clientName), 0);

        clientName[nameSize] = '\0'; // Добавление нультерминатора
        //Информация о подключенном клиенте
        std::cout << "Connected: " << clientName << " on port " << service << std::endl;
        //Сохранение в массив подключенных клиентов
        clients.push_back({ clientSocket, std::string(clientName) });
        //Запуск функции для обработки взаимодействия с клиентами
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