#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#pragma comment(lib, "Ws2_32.lib")
using namespace std;
int main()
{
	const char IP_SERV[] = "127.0.0.1";	// IP-адрес локального сервера
	const int PORT_NUM = 3000;		//Порт сервера
	const short BUFF_SIZE = 1024;// Максимальный размер буфера для обмена информацией между сервером и клиентом	
	int erStat;	//Сохранение статуса ошибок сокета
	//IP в строковом формате в числовой формат для функций сокета. Данные находятся в «ip_to_num»
	in_addr ip_to_num;
	erStat = inet_pton(AF_INET, IP_SERV, &ip_to_num);

	if (erStat <= 0) {
		cout << "Error in IP translation to special numeric format" << endl;
		return 1;
	}
	// Инициализация WinSock
	WSADATA wsData;

	erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

	if (erStat != 0) {
		cout << "Error WinSock version initializaion #";
		cout << WSAGetLastError();
		return 1;
	}
	else
		cout << "WinSock initialization is OK" << endl;

	// Инициализация сокета сервера
	SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ServSock == INVALID_SOCKET) {
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Server socket initialization is OK" << endl;

	// Привязка сокета сервера
	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));	//Инициализация структуры servInfo

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(PORT_NUM);

	erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (erStat != 0) {
		cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Binding socket to Server info is OK" << endl;

	// Начало прослушивания клиентов
	erStat = listen(ServSock, SOMAXCONN);

	if (erStat != 0) {
		cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Listening..." << endl;
	}

	//Создание и принятие клиентского сокета в случае подключения
	sockaddr_in clientInfo;
	ZeroMemory(&clientInfo, sizeof(clientInfo));	//Инициализация структуры clientInfo

	int clientInfo_size = sizeof(clientInfo);

	SOCKET ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);

	if (ClientConn == INVALID_SOCKET) {
		cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		closesocket(ClientConn);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Connection to a client established successfully" << endl;
		char clientIP[22];
		/*Преобразование IP-адреса подключенного клиента в стандартный строковый формат.*/
		inet_ntop(AF_INET, &clientInfo.sin_addr, clientIP, INET_ADDRSTRLEN);	
		cout << "Client connected with IP address " << clientIP << endl;

	}

	//Обмен текстовыми данными между Сервером и Клиентом. Отключение, если клиент отправляет «xxx»
	// Создание буферов для отправки и получения данных
	vector <char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);
	// Размер отправляемого/принимаемого пакета в байтах
	short packet_size = 0;

	while (true) {
		// Получение пакета от клиента. Программа ожидает (системная пауза) пока не получит
		packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);					
		cout << "Client's message: " << servBuff.data() << endl;
		cout << "Your (host) message: ";
		fgets(clientBuff.data(), clientBuff.size(), stdin);

		// Проверка, является ли сервер инициатором отмены
		if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
			shutdown(ClientConn, SD_BOTH);
			closesocket(ServSock);
			closesocket(ClientConn);
			WSACleanup();
			return 0;
		}

		packet_size = send(ClientConn, clientBuff.data(), clientBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			cout << "Can't send message to Client. Error # " << WSAGetLastError() << endl;
			closesocket(ServSock);
			closesocket(ClientConn);
			WSACleanup();
			return 1;
		}

	}

	closesocket(ServSock);
	closesocket(ClientConn);
	WSACleanup();

	return 0;
}

