#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
char* slice(char* s, int from, int to)
{
    memmove(s, s + from, to - from + 1);
    s[to - from + 1] = 0;
    return s;
};
int main() {
    DWORD bytesRead;
    CHAR buffer[256];
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

    if (!ReadFile(hStdin, buffer, sizeof(buffer), &bytesRead, NULL)) {
        std::cerr << "ReadFile failed" << std::endl;
        return 1;
    }
    std::cout << "Message received from parent process: " << slice(buffer, 0, bytesRead-1) << std::endl;
    return 0;
}




//#include <windows.h>
//#include <stdio.h>
//#include <iostream>
//#define BUFSIZE 4096 
//
//int main(void)
//{
//    CHAR chBuf[BUFSIZE];
//    DWORD dwRead, dwWritten;
//    HANDLE hStdin, hStdout;
//    BOOL bSuccess;
//
//    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
//    hStdin = GetStdHandle(STD_INPUT_HANDLE);
//    if (
//        (hStdout == INVALID_HANDLE_VALUE) ||
//        (hStdin == INVALID_HANDLE_VALUE)
//        )
//        ExitProcess(1);
//
//    // Send something to this process's stdout using printf.
//    printf("\n ** This is a message from the child process. ** \n");
//
//
//    for (;;)
//    {
//   
//        bSuccess = ReadFile(hStdin, chBuf, BUFSIZE, &dwRead, NULL);
//
//        if (!bSuccess || dwRead == 0)
//            break;
//
//        bSuccess = WriteFile(hStdout, chBuf, dwRead, &dwWritten, NULL);
//        std::cout << chBuf;
//        if (!bSuccess)
//            break;
//    }
//    return 0;
//}