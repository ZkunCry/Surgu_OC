#include <windows.h>
#include <iostream>
#include <string>

int main() {
    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES saAttr;
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    DWORD dwWritten;
    ZeroMemory(&siStartInfo, sizeof(siStartInfo));
    siStartInfo.cb = sizeof(siStartInfo
        );
    ZeroMemory(&piProcInfo, sizeof(piProcInfo));
    // Создание анонимного канала
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    if (!CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 0)) {
        std::cerr << "Error creating pipe.\n";
        return 1;
    }

    // Установка стандартного потока ввода для процесса-потомка
    GetStartupInfo(&siStartInfo);
    siStartInfo.hStdError = hWritePipe;
    siStartInfo.hStdOutput = hWritePipe;
    siStartInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    std::wstring sourceString = L"C:\\Users\\Eugene\\source\\repos\\ChildProcess\\Debug\\ChildProcess.exe"; // исходная строка типа std::wstring
    LPWSTR some = const_cast<LPWSTR>(sourceString.c_str()); // создание экземпляра LPWSTR из строки std::wstring
    // Запуск процесса-потомка
    if (!CreateProcess(nullptr, some, nullptr, nullptr, TRUE, 0, nullptr, nullptr, &siStartInfo, &piProcInfo)) {
        std::cerr << "Error creating child process.\n";
        return 1;
    }

    // Передача данных через канал процессу-потомку
    std::string message = "Hello, child process!";
    WriteFile(hWritePipe, message.c_str(), sizeof(message.c_str()), &dwWritten, nullptr);
    CloseHandle(hWritePipe);
    WaitForSingleObject(piProcInfo.hProcess, INFINITE);

    // Завершение работы
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);
    CloseHandle(hReadPipe);

    return 0;
}