#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <windows.h>
#include <string.h>
#include <vector>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)


std::vector<SOCKET> Sockets; //вектор для сокетов
std::vector<bool> Sockets_is_sleep;
int n;
bool is_delete = false;
HANDLE hSemaphore;
HANDLE pool[2] = {
        CreateEvent(NULL, FALSE, FALSE, L"availible"),
        CreateEvent(NULL, FALSE, FALSE, L"exit")
};

std::string toHex(int num)
{
    std::string hex;
    const char hex_digits[] = "0123456789ABCDEF";  // Массив шестнадцатеричных цифр

    while (num > 0)
    {
        hex.insert(0, 1, hex_digits[num % 16]);  // Добавляем шестнадцатеричную цифру в начало строки в зависимости от остатка от деления на 16
        num /= 16;                                // Делим число на 16
    }

    return hex;
}

std::string toBin(int num)
{
    std::string binary;

    while (num > 0)
    {
        binary.insert(0, 1, (num % 2) + '0');  // Добавляем '0' или '1' в начало строки в зависимости от остатка от деления на 2
        num /= 2;                               // Делим число на 2
    }

    return binary;
}

std::string getResult(int num, int base) {
	std::string result;
    switch (base)
    {
    case 2:
        result = toBin(num);
        break;
    case 16:
        result = toHex(num);
        break;
    default:
        result = "Неизвестная система счисления";
    }
    return result;
}

DWORD WINAPI ServerThread(int current_socket)
{
    WaitForSingleObject(hSemaphore, INFINITY);
    int continueFlag = 1;
    SOCKET lastConnectedSocket = Sockets[current_socket];
    while (continueFlag) {
        // Получение числа и системы счисления от клиента
        int num;
        int base;

        if (recv(lastConnectedSocket, (char*)&num, sizeof(num), 0) == SOCKET_ERROR)
        {
            std::cout << "Ошибка при получении данных" << std::endl;
            exit(1);
        }
        if (recv(lastConnectedSocket, (char*)&base, sizeof(base), 0) == SOCKET_ERROR)
        {
            std::cout << "Ошибка при получении данных" << std::endl;
            exit(1);
        }

        // Преобразование числа в другую систему счисления
        std::cout << "Преобразуем " << num << " в " << base << "ичную систему счисления" << std::endl;
        std::string result = getResult(num, base);
        std::cout << "Результат: " << result << std::endl;

        if (send(lastConnectedSocket, result.c_str(), result.length(), 0) == SOCKET_ERROR)
        {
            std::cout << "Ошибка при отправке данных" << std::endl;
            exit(1);

        }
        if (recv(lastConnectedSocket, (char*)&continueFlag, sizeof(continueFlag), 0) == SOCKET_ERROR)
        {
            std::cout << "Ошибка при получении данных" << std::endl;
            exit(1);
        }
        if (continueFlag == 2) {
            break;
        }
    }
    if (continueFlag == 2) {
        std::cout << "Номер current_socket перед удалением " << current_socket << std::endl;
        closesocket(Sockets[current_socket]);
        std::cout << "Клиент отключился." << std::endl;
        std::cout << Sockets.size() << std::endl;
        Sockets.erase(Sockets.begin() + current_socket);
        std::cout << Sockets.size() << std::endl;
        std::cout << Sockets_is_sleep.size() << std::endl;
        Sockets_is_sleep.erase(Sockets_is_sleep.begin() + current_socket);
        std::cout << Sockets_is_sleep.size() << std::endl;
        is_delete = true;
    }
    else
    {    
        std::cout << "Клиент " << lastConnectedSocket << " приостановил свою работу" << std::endl;
        Sockets_is_sleep[current_socket] = 1;
    }
    std::cout << "Отправляю сигнал семафору о освобождении" << std::endl;
    ReleaseSemaphore(hSemaphore, 1, NULL);
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

void start_client()
{
    const char* navigation_command = "cd D:\\OneDrive\\Документы\\GitHub\\OsLaba5\\Client\\x64\\Debug";
    const char* run_command = "start D:\\OneDrive\\Документы\\GitHub\\OsLaba5\\Client\\x64\\Debug\\Client.exe";
    // проверка на случай непредвиденных обстоятельств
    int navigation_failure = system(navigation_command);
    if (navigation_failure)
    {
        std::cout << "Failure in system navigation!" << '\n';
        exit(1);
    }
    // запускаем собранную программу у клиента
    system(run_command);
}

int inputCorrectInteger(std::string text)

{
    int polynom_degree;
    std::cout << text << std::endl;
	int leftBorder = 1;
    int rightBorder = 5;
    while (!(std::cin >> polynom_degree) || (std::cin.peek() != '\n') || (polynom_degree < leftBorder || polynom_degree > rightBorder)) {
        std::cin.clear();
        while (std::cin.get() != '\n');
        std::cout << "Ошибка :c Введите корректное целое число..." << '\n';
    }
    return polynom_degree;
}

int main()
{
	setlocale(LC_ALL, "Russian");
    // Инициализация Winsock
	initWSA();
    n = inputCorrectInteger("Введите количество клиентов для запуска: ");

    // Создание сокета
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cout << "Ошибка при создании сокета" << std::endl;
        WSACleanup();
        return -1;
    }
    std::cout << "Сокет сервера запущен успешно!" << std::endl;

    // Настройка сокета
    sockaddr_in serverAddr;
	int serverSize = sizeof(serverAddr);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(5555);
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cout << "Ошибка при настройке сокета" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

	// Ожидание прослушивания с максимальной длинной очереди
	std::cout << "Ожидание подключения клиента..." << std::endl;
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cout << "Ошибка при ожидании подключения" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }



    // Создание семафора и события
    hSemaphore = CreateSemaphore(NULL, 1, n, L"semaphore");

    for (int i = 0; i < n; i++) {
        start_client();
    }


    int current_socket = 0;
    SOCKET lastConnectedSocket;
    bool flag = true;
    int iteration = 0;
    while (flag) {
        if (iteration < n) {
            std::cout << "Жду подключений" << std::endl;
            Sockets.push_back(accept(serverSocket, (SOCKADDR*)&serverAddr, &serverSize));
            Sockets_is_sleep.push_back(0);
            if (Sockets[Sockets.size() - 1] == INVALID_SOCKET)
            {
                std::cout << "Ошибка при принятии подключения" << std::endl;
                closesocket(serverSocket);
                WSACleanup();
                return -1;
            }
            std::cout << "Клиент " << Sockets[Sockets.size() - 1] << " подключился успешно!" << std::endl;
        }
        else {
            current_socket=0;
        }
        if (Sockets.size() == 0) {
            flag = false;
            break;
        }
        if (Sockets_is_sleep[current_socket] == 1) {
            std::cout << "Снимаю приостановку с клиента " << Sockets[current_socket] << std::endl;
            SetEvent(pool[1]);
        }
        std::cout << "Номер сокета, с которым мы будем работать: " << Sockets[current_socket] << std::endl;
 
        lastConnectedSocket = Sockets[current_socket];
     
        // Создание потока сервера
        HANDLE hThread = CreateThread(NULL,NULL, (LPTHREAD_START_ROUTINE)ServerThread, (LPVOID)current_socket, NULL, NULL);
        if (hThread == NULL)
        {
            std::cout << "Ошибка при создании потока" << std::endl;
            closesocket(lastConnectedSocket);
        }
        DWORD dwWaitResult =  WaitForSingleObject(hThread, INFINITE);

        CloseHandle(hThread);
        if (!is_delete) {
            current_socket++;
        }
        if (current_socket == n) {
            current_socket = 0;
        }
        
        is_delete = false;
        iteration++;
        if (Sockets.size() == 0 && iteration == n) {
            flag = false;
        }
        std::cout << "==================================================================================" << std::endl;

    }
    std::cout << "Корректное завершение работы, верьте мне" << std::endl;
    // Освобождение ресурсов
    CloseHandle(hSemaphore);
    for (int i = 0; i < 2; i++) {
        CloseHandle(pool[i]);
    }
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
