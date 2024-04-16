#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#pragma warning(disable:4996)

int inputCorrectInteger(std::string text)

{
    int polynom_degree;
    std::cout << text << std::endl;
    while (!(std::cin >> polynom_degree) || (std::cin.peek() != '\n')) {
        std::cin.clear();
        while (std::cin.get() != '\n');
        std::cout << "Ошибка :c Введите корректное целое число..." << '\n';
    }
    return polynom_degree;
}

int inputCorrectBase(std::string text)
{
    int polynom_degree;
    std::cout << text << std::endl;
    while (!(std::cin >> polynom_degree) || (std::cin.peek() != '\n') || ((polynom_degree !=2) && (polynom_degree != 16))) {
        std::cin.clear();
        while (std::cin.get() != '\n');
        std::cout << "Ошибка :c Введите корректное целое число..." << '\n';
    }
    return polynom_degree;
}

int continueCheck()
/*
Функция ждет ввод пользователя.
Если пользователь вводит Y или y, функция возвращает 1.
Если пользователь вводит N или n, функция возвращает 0.
Если пользователь вводит не символы выше, проверка зацикливается и будет ждать символы выше.
*/
{
    bool flag = true;
    char symbol;
    std::cout << "Хотите продолжить(Y/y) или преостановить(N/n) или отключить (S/s) программу ?" << '\n';
    while (!(std::cin >> symbol) || flag)
    {
        std::cin.clear();
        if ((symbol == 'N' || symbol == 'n') && (std::cin.peek() == '\n'))
        {
            return 0;
        }
        else if ((symbol == 'Y' || symbol == 'y') && (std::cin.peek() == '\n'))
        {
            return 1;
        }
        else if ((symbol == 'S' || symbol == 's') && (std::cin.peek() == '\n'))
        {
            return 2;
        }
        while (std::cin.get() != '\n');
        std::cout << "Ошибка :c Для работы доступны только продолжение (Y/y) прекращение (N/n) и (S/s) отключение программы." << std::endl;
    }
}


HANDLE hSemaphore;
HANDLE pool[2] = {
    OpenEvent(EVENT_ALL_ACCESS, FALSE, L"availible"),
    OpenEvent(EVENT_ALL_ACCESS, FALSE, L"exit")
};

DWORD WINAPI ClientThread(LPVOID lpParam)
{
    // Получение сокета из параметра
    SOCKET clientSocket = (SOCKET)lpParam;
    bool is_iterated = true;
    while (is_iterated)
    {
        std::cout << "Дождался! Начинем общение с сервером" << std::endl;

        // Ввод числа и системы счисления

        int num = inputCorrectInteger("Введите число для перевода: ");
        int base = inputCorrectBase("Введите систему счисления (2, 16)");

        // Отправка числа и системы счисления на сервер
        std::cout << "Отправляю число " << num << " и систему счисления " << base << " на сервер" << std::endl;
        if (send(clientSocket, (char*)&num, sizeof(num), 0) == SOCKET_ERROR)
        {
            std::cout << "Ошибка при отправке данных" << std::endl;
            break;
        }
        if (send(clientSocket, (char*)&base, sizeof(base), 0) == SOCKET_ERROR)
        {
            std::cout << "Ошибка при отправке данных" << std::endl;
            break;
        }

        // Получение результата от сервера
        char serverResponse [1024];
        memset(serverResponse, 0, 1024);
        if (recv(clientSocket, serverResponse, sizeof(serverResponse), 0) == SOCKET_ERROR)
        {
            std::cout << "Ошибка при получении данных" << std::endl;
            break;
        }

        // Вывод результата
        std::cout << "Получаю ответ сервера: " << serverResponse << std::endl;

        memset(serverResponse, 0, 1024);

        int continueFlag = continueCheck();
        if (send(clientSocket, (char*)&continueFlag, sizeof(continueFlag), 0) == SOCKET_ERROR)
        {
            std::cout << "Ошибка при отправке данных" << std::endl;
            break;
        }
        if (continueFlag == 1)
        {
            std::cout << "Продолжаю работу" << std::endl;
        }
        else if(continueFlag == 0) {
            std::cout << "Приостанавливаю свою работу" << std::endl;
            DWORD dwWaitResult = WaitForSingleObject(pool[1], INFINITE);
            switch (dwWaitResult) {
            case WAIT_OBJECT_0:
                std::cout << "Сигнал для снятия с приостановки обнаружен!" << std::endl;
                break;
            case WAIT_ABANDONED:
                std::cout << "Adandoned." << std::endl;
                break;
            case WAIT_TIMEOUT:
                std::cout << "TIMEOUT." << std::endl;
                break;
            case WAIT_FAILED:
                std::cout << "FAIL." << std::endl;
                std::cout << GetLastError() << std::endl;
                break;
            default:
                std::cout << "Чето случилось.." << GetLastError() << std::endl;
            }
            std::cout << "Приостановка работы завершена" << std::endl;
        }
		else if (continueFlag == 2)
		{
			std::cout << "Прекращаю свою работу." << std::endl;
			is_iterated = false;
		}
    }

    return 0;
}

void initWSA()
{
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0)
    {
        std::cout << "Error" << std::endl;
        exit(1);
    }
}

SOCKET createClientSocket() {
    // Создание сокета
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cout << "Ошибка при создании сокета" << std::endl;
        WSACleanup();
        return -1;
    }
    return clientSocket;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    initWSA();
    SOCKET clientSocket = createClientSocket();

    // Подключение к серверу
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(5555);
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cout << "Ошибка при подключении к серверу" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    //Открытие семафора
    hSemaphore = OpenSemaphore(SYNCHRONIZE, FALSE, L"semaphore");
    if (hSemaphore == NULL) {
        std::cerr << "Ошибка при открытии семафора" << std::endl;
    }

    if (pool[0] == NULL) {
        std::cerr << "Ошибка при открытии флага 0" << std::endl;
    }
    if (pool[1] == NULL) {
        std::cerr << "Ошибка при открытии флага 1" << std::endl;
    }
   
    // Ожидание события
    std::cout << "Нахожусь в режиме ожидания..." << std::endl;

    DWORD dwWaitResult = WaitForSingleObject(hSemaphore, INFINITE);
    // Создание потока клиента
    HANDLE hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)clientSocket, 0, NULL);
    if (hThread == NULL)
    {
        std::cout << "Ошибка при создании потока" << std::endl;
        CloseHandle(pool);
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }
    // Ожидание завершения потока
    WaitForSingleObject(hThread, INFINITE);

    // Освобождение ресурсов
    CloseHandle(hThread);
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
