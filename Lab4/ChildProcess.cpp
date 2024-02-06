#include <windows.h>
#include <iostream>
#include <string>

int main() {
    HANDLE hReadPipe;
    DWORD dwRead;
    char buffer[1000];

    hReadPipe = GetStdHandle(STD_INPUT_HANDLE);

    // Получение данных от процесса-родителя через стандартный поток ввода
    ReadFile(hReadPipe, buffer, sizeof(buffer), &dwRead, nullptr);

    // Форматированный вывод данных в стандартный поток вывода
    std::cout << "Received from parent process: " << buffer << std::endl;

    return 0;
}