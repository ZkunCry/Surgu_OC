
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <windows.h>
#include <chrono>
std::vector<int> data(10000); // массив для сортировки
std::vector<int> sortedData1, sortedData2; // массивы для хранения отсортированных данных

// Функция заполнения массива случайными числами
void fillArrayWithRandomNumbers() {

    std::srand(static_cast<unsigned int>(std::time(0)));
    for (auto& el : data) {
        el = std::rand() % 1000;
    }
}

// Функция сортировки массива пузырьковым методом
void bubbleSort() {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> temp(data);
    bool swapped;
    for (size_t i = 0; i < temp.size() - 1; ++i) {
        swapped = false;
        for (size_t j = 0; j < temp.size() - i - 1; ++j) {
            if (temp[j] > temp[j + 1]) {
                std::swap(temp[j], temp[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Время выполнения потока bubblesort: " << duration.count() << " мс" << std::endl;
    sortedData1 = temp;
}

// Функция сортировки массива быстрым методом
void quickSort(std::vector<int>& arr, int left, int right) {
    int i = left, j = right;
    int pivot = arr[(left + right) / 2];

    while (i <= j) {
        while (arr[i] < pivot) {
            i++;
        }
        while (arr[j] > pivot) {
            j--;
        }
        if (i <= j) {
            std::swap(arr[i], arr[j]);
            i++;
            j--;
        }
    }

    if (left < j) {
        quickSort(arr, left, j);
    }
    if (i < right) {
        quickSort(arr, i, right);
    }
}

// Функция-обертка для вызова быстрого метода сортировки
void quickSortWrapper() {
    std::vector<int> temp(data);
    auto start = std::chrono::high_resolution_clock::now();
    quickSort(temp, 0, temp.size() - 1);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
     std:: cout << "Время выполнения потока quickSort: " << duration.count() << " мс" << std::endl;

    sortedData2 = temp;
}

int main() {
    system("chcp 1251>nul");
    fillArrayWithRandomNumbers(); // заполнение массива случайными числами

    // Создание потоков для сортировки
    HANDLE thread1 = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)bubbleSort, nullptr, CREATE_SUSPENDED, nullptr);
    HANDLE thread2 = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)quickSortWrapper, nullptr, CREATE_SUSPENDED, nullptr);
    std::cout << "Потоки с разным приоритетом: bubblesort имеет максимальный, quicksort минимальный " << std::endl;

    // Установка различных приоритетов потокам
    SetThreadPriority(thread1, THREAD_PRIORITY_HIGHEST); // максимальный приоритет
    SetThreadPriority(thread2, THREAD_PRIORITY_LOWEST); // минимальный приоритет
  
    // Запуск дополнительных потоков
    ResumeThread(thread1);
    ResumeThread(thread2);

    // Ожидание завершения потоков
    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);
    CloseHandle(thread1);
    CloseHandle(thread2);
  
    std::cout << "Потоки с разным приоритетом: bubblesort имеет минимальный, quicksort максимальный " << std::endl;
    fillArrayWithRandomNumbers(); // заполнение массива случайными числами второй раз

    HANDLE thread3 = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)bubbleSort, nullptr, CREATE_SUSPENDED, nullptr);
    HANDLE thread4 = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)quickSortWrapper, nullptr, CREATE_SUSPENDED, nullptr);
    // Переключение приоритетов потоков
    SetThreadPriority(thread3, THREAD_PRIORITY_LOWEST); // минимальный приоритет
    SetThreadPriority(thread4, THREAD_PRIORITY_HIGHEST); // максимальный приоритет

    ResumeThread(thread3);
    ResumeThread(thread4);

    WaitForSingleObject(thread3, INFINITE);
    WaitForSingleObject(thread4, INFINITE);

    CloseHandle(thread3);
    CloseHandle(thread4);
    std::cout << "Блокирование динамических изменений" << std::endl;
    fillArrayWithRandomNumbers(); // заполнение массива случайными числами третий раз

    HANDLE thread5 = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)bubbleSort, nullptr, CREATE_SUSPENDED, nullptr);
    HANDLE thread6 = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)quickSortWrapper, nullptr, CREATE_SUSPENDED, nullptr);
    // Блокирование динамических изменений приоритетов потоков
    SetThreadPriority(thread5, THREAD_PRIORITY_NORMAL); // блокирование минимального приоритета
    SetThreadPriority(thread6, THREAD_PRIORITY_NORMAL); // блокирование максимального приоритета
    ResumeThread(thread5);
    ResumeThread(thread6);
    WaitForSingleObject(thread5, INFINITE);
    WaitForSingleObject(thread6, INFINITE);
    CloseHandle(thread5);
    CloseHandle(thread6);
    return 0;
}


