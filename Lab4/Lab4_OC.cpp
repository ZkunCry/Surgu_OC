#include <Windows.h>
#include <iostream>
#include <string>

int main() {
    HANDLE hChildStdinRd, hChildStdinWr;
    SECURITY_ATTRIBUTES saAttr;
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&hChildStdinRd, &hChildStdinWr, &saAttr, 0)) {
        std::cerr << "CreatePipe failed" << std::endl;
        return 1;
    }

    GetStartupInfo(&siStartInfo);
    siStartInfo.hStdInput = hChildStdinRd;
    siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    siStartInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    std::string message = "Hello from parent process!!!";
    DWORD bytesWritten;
    WriteFile(hChildStdinWr, message.c_str(), message.size(), &bytesWritten, NULL);

    if (!CreateProcess(L"ChildProcess.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo)) {
        std::cerr << "CreateProcess failed" << std::endl;
        return 1;
    }

    CloseHandle(hChildStdinWr);
    WaitForSingleObject(piProcInfo.hProcess, INFINITE);

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);
    CloseHandle(hChildStdinRd);

    return 0;
}