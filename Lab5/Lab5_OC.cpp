﻿#include <windows.h> 
#include <iostream> 
#include "buffer.h"
#include <semaphore>
#define cProducers 5 /*Количество производителей*/ 
#define cConsumers 5 /*Количество потребителей*/ 
#define BufferSize 5 /*Размер буфера*/

int cOperations = 100; /*Количество операций над буфером*/
DWORD __stdcall getkey(void* b) {
	std::cin.get();
	return cOperations = 0;
}
//____________________________________________________________________________________________________________
HANDLE hSemaphores[3];
//____________________________________________________________________________________________________________

/* Изменения в программе должны осуществляться только в глав- ном модуле и только путем добавления кода,
позволяющего синхронизировать действия:
-	внутри "производителей" и "потребителей";
-	в теле главной функции (создание объектов синхронизации и пр.);
-	в глобальном пространстве (объявление переменных и пр.).
*/

/*Исходный вариант потока-производителя*/
DWORD __stdcall producer(void* b) {
	while (cOperations-- > 0) {
		int item = rand();
		//____________________________________________________________________________________________________________
		WaitForSingleObject(hSemaphores[1], INFINITE); //указатель на семафор, ожидание
		WaitForSingleObject(hSemaphores[2], INFINITE); //указатель на семафор, ожидание
		//____________________________________________________________________________________________________________
		((Buffer*)b)->PutItem(item);
		//____________________________________________________________________________________________________________
		ReleaseSemaphore(hSemaphores[2], 1, NULL); //указатель на семафор, изменяет счетчик на 1
		//ReleaseSemaphore(hSemaphores[0], 1, NULL);//указатель на семафор, изменяет счетчик на 1
		//____________________________________________________________________________________________________________
		Sleep(500 + rand() % 100);
	}
	return 0;
}

/*Исходный вариант потока-потребителя*/
DWORD __stdcall consumer(void* b) {
	while (cOperations-- > 0) {
		//____________________________________________________________________________________________________________
	//	WaitForSingleObject(hSemaphores[0], INFINITE);//указатель на семафор, ожидание
		WaitForSingleObject(hSemaphores[2], INFINITE);//указатель на семафор, ожидание
		//____________________________________________________________________________________________________________
		std::cout << ((Buffer*)b)->GetItem() << std::endl;
		//____________________________________________________________________________________________________________
		ReleaseSemaphore(hSemaphores[2], 1, NULL);//указатель на семафор, изменяет счетчик на 1
		ReleaseSemaphore(hSemaphores[1], 1, NULL);//указатель на семафор, изменяет счетчик на 1
		//____________________________________________________________________________________________________________
		Sleep(500 + rand() % 100);
	}
	return 0;
}

int main() {
	Buffer* Buf = Buffer::CreateBuffer(5); /*Создание буфера*/
	HANDLE hThreads[cProducers + cConsumers];
	/*Вспомогательный поток, ожидающий нажатие клавиши*/
	CreateThread(0, 0, getkey, 0, 0, 0);
	//____________________________________________________________________________________________________________
	//hSemaphores[0] = CreateSemaphore(NULL, 0, BufferSize, NULL);  //атрибут, начальное состояние, максимальное состояние, имя
	hSemaphores[1] = CreateSemaphore(NULL, BufferSize, BufferSize, NULL);
	hSemaphores[2] = CreateSemaphore(NULL, 1, 1, NULL);
	//____________________________________________________________________________________________________________
	for (int i = 0; i < cProducers; i++)
		/*Создание потоков-производителей*/
		hThreads[i] = CreateThread(0, 0, producer, Buf, 0, 0);
	for (int i = cProducers; i < cProducers + cConsumers; i++)
		/*Создание потоков-потребителей*/
		hThreads[i] = CreateThread(0, 0, consumer, Buf, 0, 0);

	WaitForMultipleObjects(cProducers + cConsumers, hThreads, true, INFINITE);

	std::cin.get();
	return 0;
}
