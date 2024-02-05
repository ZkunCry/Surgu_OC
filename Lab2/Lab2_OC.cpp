#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <psapi.h>

int main() {
    system("chcp 1251 >nul");
    // Создание процесса
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    if (!CreateProcess(_T("C:\\Windows\\System32\\notepad.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cout << "Ошибка создания процесса: " << GetLastError() << std::endl;
        return 1;
    }

    // Получение идентификатора созданного процесса
    DWORD processId = GetProcessId(pi.hProcess);
    std::cout << "Идентификатор созданного процесса: " << processId << std::endl;

    // Проверка уникальности дескрипторов процессов
    if (pi.hProcess == pi.hThread) {
        std::cout << "Дескрипторы процесса и потока одинаковы!" << std::endl;
    }
    else {
        std::cout << "Дескрипторы процесса и потока разные." << std::endl;
    }

    // Получение значений временных характеристик процесса
    FILETIME createTime, exitTime, kernelTime, userTime;
    if (GetProcessTimes(pi.hProcess, &createTime, &exitTime, &kernelTime, &userTime)) {
        SYSTEMTIME st;
        FileTimeToSystemTime(&createTime, &st);
        std::cout << "Время создания процесса: " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << std::endl;
    }
    else {
        std::cout << "Ошибка получения временных характеристик процесса: " << GetLastError() << std::endl;
    }

    // Завершение процесса и закрытие открытых ранее дескрипторов
    if (!TerminateProcess(pi.hProcess, 0)) {
        std::cout << "Ошибка завершения процесса: " << GetLastError() << std::endl;
    }

    // Закрытие открытых дескрипторов
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Проверка успешности выполнения операций над процессами
    if (GetLastError() == 0) {
        std::cout << "Операции над процессами выполнены успешно." << std::endl;
    }
    else {
        std::cout << "При выполнении операций над процессами произошла ошибка: " << GetLastError() << std::endl;
    }

    return 0;
}