#include <iostream>
#include <Windows.h>
#include <cstring>

int main() {
    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES saAttr;
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    if (!CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 0)) {
        std::cerr << "Error creating pipe." << std::endl;
        return 1;
    }

    char message[] = "Hello world!";


    GetStartupInfo(&siStartInfo);
    std::wstring sourceString = L"C:\\Users\\Eugene\\source\\repos\\ChildProcess\\Debug\\ChildProcess.exe"; // исходная строка типа std::wstring
    LPWSTR some = const_cast<LPWSTR>(sourceString.c_str()); // создание экземпляра LPWSTR из строки std::wstring
    if (!CreateProcess(some, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo)) {
        std::cerr << "Error creating child process." << std::endl;
        return 1;
    }

    DWORD dwWritten;
   auto result =  WriteFile(hWritePipe, message, strlen(message) + 1, &dwWritten, NULL);
    CloseHandle(hWritePipe);
      
    WaitForSingleObject(piProcInfo.hProcess, INFINITE);

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);
    CloseHandle(hReadPipe);

    return 0;
}



std::wstring sourceString = L"C:\\Users\\Eugene\\source\\repos\\ChildProcess\\Debug\\ChildProcess.exe"; // исходная строка типа std::wstring
LPWSTR some = const_cast<LPWSTR>(sourceString.c_str()); // создание экземпляра LPWSTR из строки std::wstring