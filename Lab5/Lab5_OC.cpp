	#include <windows.h> 
	#include <iostream> 
	#include "buffer.h"
	#include <semaphore>
	#define cProducers 3 /*Количество производителей*/ 
	#define cConsumers 3 /*Количество потребителей*/ 
	#define BufferSize 5 /*Размер буфера*/

	int cOperations = 100; /*Количество операций над буфером*/
	DWORD __stdcall getkey(void* b) {
		std::cin.get();
		return cOperations = 0;
	}
	HANDLE hMutex;
	/*Исходный вариант потока-производителя*/
	DWORD __stdcall producer(void* b) {
		while (cOperations-- > 0) {
			int item = rand() % 10000;

			WaitForSingleObject(hMutex, INFINITE); // Захват мьютекса
			((Buffer*)b)->PutItem(item);
			ReleaseMutex(hMutex); // Освобождение мьютекса
			//std::cout << "Produced: " << item << std::endl;
			Sleep(500 + rand() % 100);
		}
		return 0;
	}
	DWORD __stdcall consumer(void* b) {
		while (cOperations-- > 0) {
			WaitForSingleObject(hMutex, INFINITE); // Захват мьютекса
			std::cout <<"Consumed: " << ((Buffer*)b)->GetItem() << std::endl;
			ReleaseMutex(hMutex); // Освобождение мьютекса

			Sleep(500 + rand() % 100);
		}

		return 0;
	}

	int main() {
		// Создание мьютекса для синхронизации доступа к буферу
		hMutex = CreateMutex(NULL, FALSE, NULL);
		if (hMutex == NULL) {
			std::cerr << "Failed to create mutex." << std::endl;
			return -1;
		}
		Buffer* Buf = Buffer::CreateBuffer(5); // Создание буфера
		HANDLE hThreads[cProducers + cConsumers];
		// Вспомогательный поток, ожидающий нажатие клавиши
		CreateThread(0, 0, getkey, 0, 0, 0);

		for (int i = 0; i < cProducers; i++)
			hThreads[i] = CreateThread(0, 0, producer, Buf, 0, 0);

		for (int i = cProducers; i < cProducers + cConsumers; i++)
			hThreads[i] = CreateThread(0, 0, consumer, Buf, 0, 0);

		WaitForMultipleObjects(cProducers + cConsumers, hThreads, true, INFINITE);
		CloseHandle(hMutex); // Закрытие мьютекса
		std::cin.get();
		return 0;
	}
