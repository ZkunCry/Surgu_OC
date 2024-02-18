	#include <windows.h> 
	#include <iostream> 
	#include "buffer.h"
	
	#define cProducers 4 /*Количество производителей*/ 
	#define cConsumers 4 /*Количество потребителей*/ 
	#define BufferSize 5 /*Размер буфера*/

	int cOperations = 100; /*Количество операций над буфером*/
	DWORD __stdcall getkey(void* b) {
		std::cin.get();
		return cOperations = 0;
	}
	HANDLE mutex;
	HANDLE semaphoreBufferEmpty; // Семафор, указывающий на количество пустых слотов в буфере
	HANDLE semaphoreBufferFull; // Семафор, указывающий на количество заполненных слотов в буфере

	// Исходный вариант потока-производителя
	DWORD __stdcall producer(void* b) {
		while (cOperations-- > 0) {
			int item = rand()%1000 +cOperations;

			WaitForSingleObject(semaphoreBufferEmpty, INFINITE); // Ждем, пока появится свободный слот в буфере
			WaitForSingleObject(mutex, INFINITE); // Захватываем мьютекс перед доступом к буферу

			((Buffer*)b)->PutItem(item);
			cout <<"Producers " << item << endl;

			ReleaseMutex(mutex); // Освобождаем мьютекс после доступа к буферу
			ReleaseSemaphore(semaphoreBufferFull, 1, NULL); // Увеличиваем счетчик заполненных слотов

			Sleep(500 + rand() % 100);
		}
		return 0;
	}

	// Исходный вариант потока-потребителя
	DWORD __stdcall consumer(void* b) {
		while (cOperations-- > 0) {
			WaitForSingleObject(semaphoreBufferFull, INFINITE); // Ждем, пока появится заполненный слот в буфере
			WaitForSingleObject(mutex, INFINITE); // Захватываем мьютекс перед доступом к буферу

			cout <<"Consumers " << ((Buffer*)b)->GetItem() << endl;

			ReleaseMutex(mutex); // Освобождаем мьютекс после доступа к буферу
			ReleaseSemaphore(semaphoreBufferEmpty, 1, NULL); // Увеличиваем счетчик пустых слотов

			Sleep(500 + rand() % 100);
		}
		return 0;
	}

	int main() {
		// Создание мьютекса для синхронизации доступа к буферу
		mutex = CreateMutex(NULL, FALSE, NULL);
		if (mutex == NULL) {
			std::cerr << "Failed to create mutex." << std::endl;
			return -1;
		}
		semaphoreBufferEmpty = CreateSemaphore(NULL, BufferSize, BufferSize, NULL); // Создание семафора для пустых слотов
		semaphoreBufferFull = CreateSemaphore(NULL, 0, BufferSize, NULL); // Создание семафора для заполненных слотов
		Buffer* Buf = Buffer::CreateBuffer(BufferSize); // Создание буфера
		HANDLE hThreads[cProducers + cConsumers];
		// Вспомогательный поток, ожидающий нажатие клавиши
		CreateThread(0, 0, getkey, 0, 0, 0);

		for (int i = 0; i < cProducers; i++)
			hThreads[i] = CreateThread(0, 0, producer, Buf, 0, 0);

		for (int i = cProducers; i < cProducers + cConsumers; i++)
			hThreads[i] = CreateThread(0, 0, consumer, Buf, 0, 0);

		WaitForMultipleObjects(cProducers + cConsumers, hThreads, true, INFINITE);
		CloseHandle(mutex); // Закрытие мьютекса
		CloseHandle(semaphoreBufferEmpty); // Закрываем семафор для пустых слотов
		CloseHandle(semaphoreBufferFull); // Закрываем семафор для заполненных слотов
		std::cin.get();
		return 0;
	}
