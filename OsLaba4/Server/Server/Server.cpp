#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#include <fstream>
#include <string>

#pragma warning(disable: 4996)

SOCKET Connections[100];
int connectionsCounter = 0;
const char* CD_COMMAND = "cd D:\\OneDrive\\Документы\\GitHub\\OsLaba4\\Client\\x64\\Debug";
const char* START_COMMAND = "start D:\\OneDrive\\Документы\\GitHub\\OsLaba4\\Client\\x64\\Debug\\Client.exe";




void initLibrary()
{
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0)
	{
		std::cout << "Error loading WSA" << std::endl;
		exit(1);
	}
}


void startClientFromTerminal()
{

	// проверка на случай непредвиденных обстоятельств
	int command_not_execute = system(CD_COMMAND);
	if (command_not_execute)
	{
		std::cout << "Failed to start command" << '\n';
		exit(1);
	}
	system(START_COMMAND);
}


void giveWordsToClient(SOCKET newConnection, int amount_of_clients, int i, std::string filename) {
	std::ifstream file(filename);
	std::string word;
	int count = 0;

	while (file >> word) {
		if (count % amount_of_clients == i) {
			std::cout << "[SEND] " <<"[" << word <<"]" << " [ TO " << newConnection << "]" << std::endl;
			int result = send(newConnection, word.c_str(), word.length(), NULL);
			if (result == SOCKET_ERROR)
			{
				std::cout << "Error with sending word: " << word << std::endl;
				std::cout <<"Error code: " << WSAGetLastError() << std::endl;
				exit(1);
			}
		}
		count++;
	}
	file.close();
}

void createAndConnect(SOCKET sListen, SOCKADDR_IN addr, int sizeofaddr, int amount_of_clients, std::string filename)
{
	SOCKET newConnection;
	for (int i = 0; i < amount_of_clients; ++i)
	{
		startClientFromTerminal();
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);
		if (newConnection == 0)
		{
			std::cout << "Socket Connection Error" << std::endl;
		}
		else
		{
			std::cout << "Client connected Successfully!" << std::endl;
			giveWordsToClient(newConnection, amount_of_clients, i, filename);
		}
		Connections[i] = newConnection;
		++connectionsCounter;
	}
}



int getSocketNumber()
{
	int clientNumber;
	int leftBorder = 3;
	int rightBorder = 10;
	std::cout << "Input number of Clients (n): ";
	while (!(std::cin >> clientNumber) || (std::cin.peek() != '\n') || (clientNumber < leftBorder) || (clientNumber > rightBorder)) {
		std::cin.clear();
		while (std::cin.get() != '\n');
		std::cout << "Error :c Input correct number of Clients..." << '\n';
	}
	return clientNumber;
}



bool isCorrectFileName(std::string filename) {

	std::ifstream file(filename);
	if (!file) {
		file.close();
		return false;
	}
	return true;
}


std::string getFileName() {
	std::string filename;
	std::cout << "Input filename: ";
	while (!(std::cin >> filename) || (std::cin.peek() != '\n') || !isCorrectFileName(filename)) {
		std::cin.clear();
		while (std::cin.get() != '\n');
		std::cout << "Error :c Input correct filename..." << '\n';
	}
	return filename;
}



int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	initLibrary();
	int clientNumber = getSocketNumber();
	std::string filename = getFileName();

	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	createAndConnect(sListen, addr, sizeof(addr), clientNumber, filename);
	system("pause");
	return 0;
}