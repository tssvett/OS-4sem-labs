#include<iostream>
#include<unistd.h>
#include<string>
#include<algorithm>
#include<vector>
#include<sstream>
#include <sys/stat.h>
#include<fstream>
#include<filesystem>
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

enum input_types { terminal, console };


class Command {
public:
	std::string name;
	std::vector<std::string> args;
	std::vector<std::string> avialiable_commands = { "help", "copy", "move", "info", "chmod" };

	Command() { name = "default"; }

	Command(std::string name) {
		this->name = name;
	}

	Command(std::string name, int argc, std::vector<std::string> args) {
		this->name = name;
		this->args = args;
	}

	void execute(std::vector<std::string> vector_command) {
		std::string command_name = vector_command[0];
		vector_command.erase(vector_command.begin());
		if (is_avialiable_command(command_name) && is_help_command(command_name) && is_avialible_args_help(vector_command)) {
			this->name = command_name;
			this->args = vector_command;
			execute_help();
		}
		else if (is_avialiable_command(command_name) && is_copy_command(command_name) && is_avialiable_args_copy(vector_command)) {
			this->name = command_name;
			this->args = vector_command;
			execute_copy(vector_command[0], vector_command[1]);
		}
		else if (is_avialiable_command(command_name) && is_move_command(command_name) && is_avialible_args_move(vector_command)) {
			this->name = command_name;
			this->args = vector_command;
			execute_move(vector_command[0], vector_command[1]);
		}

		else if (is_avialiable_command(command_name) && is_info_command(command_name) && is_avialible_args_info(vector_command)) {
			this->name = command_name;
			this->args = vector_command;
			execute_info(vector_command[0]);
		}

		else if (is_avialiable_command(command_name) && is_chmod_command(command_name) && is_avialible_args_chmod(vector_command)) {
			this->name = command_name;
			this->args = vector_command;
			execute_chmod(vector_command[1], vector_command[0]);
		}

		else {
			std::cout << "Команда некорректна!" << std::endl;
		}
	}

	bool is_avialiable_command(std::string names) {
		return std::find(avialiable_commands.begin(), avialiable_commands.end(), names) != avialiable_commands.end();
	}

	bool is_avialiable_args_copy(std::vector<std::string> args) {
		return args.size() == 2;
	}

	bool is_avialible_args_move(std::vector<std::string> args) {
		return args.size() == 2;
	}

	bool is_avialible_args_info(std::vector<std::string> args) {
		return args.size() == 1;
	}

	bool is_avialible_args_help(std::vector<std::string> args) {
		return args.size() == 0;
	}

	bool is_avialible_args_chmod(std::vector<std::string> args) {
		return (is_mask(args[0]) || is_correct_rwx(args[0])) && args.size() == 2;
	}


	bool is_help_command(std::string name) {
		return name == "help";

	}

	bool is_copy_command(std::string name) {
		return name == "copy";
	}

	bool is_move_command(std::string name) {
		return name == "move";
	}

	bool is_info_command(std::string name) {
		return name == "info";
	}

	bool is_chmod_command(std::string name) {
		return name == "chmod";
	}

	bool is_default_name() {
		return name == "default";
	}

private:

	bool is_mask(std::string arg) {
		bool is_correct = true;
		if (arg.size() != 3) {
			return false;
		}
		for (int i = 0; i < arg.size(); i++) {
			if (arg[i] != '0' && arg[i] != '1' && arg[i] != '2' && arg[i] != '3' && arg[i] != '4' && arg[i] != '5' && arg[i] != '6' && arg[i] != '7') {
				is_correct = false;
				return is_correct;
			}
		}
		return is_correct;
	}

	bool is_correct_rwx(std::string arg) {
		bool is_correct = false;
		if (arg.size() != 9) {
			return false;
		}
		int i = 0;
		for (int j = 0; j < 3; j++) {
			for (i = 0; i < 3; i++) {
				if (arg[0] == '-' || arg[0] == 'r') {
					is_correct = true;
				}
				else if (arg[1] == '-' || arg[1] == 'w')
				{
					is_correct = true;
				}
				else if (arg[2] == '-' || arg[2] == 'x')
				{
					is_correct = true;
				}
				else {
					is_correct = false;
					return is_correct;
				}
			}
		}
		return is_correct;


	}
	void execute_help() {
		std::cout << "Команды для управления программой:" << std::endl;
		std::cout << "help           Получение инструкции для управления" << std::endl;
		std::cout << "copy [что] [куда]                Копирование файла" << std::endl;
		std::cout << "move [что] [куда]                Перемещение файла" << std::endl;
		std::cout << "info                  Получение информации о файле" << std::endl;
		std::cout << "chmod [права/маска] [файл]        Измененние прав на файл" << std::endl;
	}

	bool execute_copy(std::string file_path, std::string new_file_path) {
		std::cout << "Копирование файла" << std::endl;
		if (file_path == new_file_path)
		{
			std::cout << "Нельзя копировать файл в себя!" << std::endl;
			return 0;
		}
		std::ifstream in;
		std::ofstream out;
		in.open(file_path, std::ios::binary);
		if (in.is_open())
		{
			const int buffer_size = 4;
			char* buffer = new char[buffer_size];
			out.open(new_file_path, std::ios::binary);
			while (!in.eof())
			{
				in.read(buffer, buffer_size);
				if (in.gcount())
				{
					out.write(buffer, in.gcount());
				}
			}
			std::string path_to_new_file = std::string(get_current_dir_name()) + "/" + new_file_path;
			std::cout << "Файл успешно скопирован по пути: " << path_to_new_file << std::endl;
			out.close();
			delete[] buffer;
		}
		else
		{
			std::cout << "Файл, который вы пытаетесь скопировать, не существует" << std::endl;
			return 0;
		}
		in.close();
	}

	bool execute_move(std::string file_name, std::string dirname) {
		std::ifstream dir(dirname);
		std::ifstream file(file_name);
		char* directory = get_current_dir_name();
		std::string path_to_new_file = std::string(directory) + "/" + dirname + "/" + file_name;
		if (!file.is_open())
		{
			std::cout << "Файл, который вы пытаетесь переместить, не существует" << std::endl;
			file.close();
			return 0;
		}
		if (file_name == dirname)
		{
			std::cout << "Нельзя переместить файл в себя!" << std::endl;
			return 0;
		}

		if (is_file(dirname)) {
			path_to_new_file = dirname;
		}

		if (dirname == ".") {
			std::string new_file_name = file_name;
			delete_string_by_delimeter(new_file_name, '/');
			path_to_new_file = std::string(directory) + "\\" + new_file_name;
		}

		struct stat sb;
		if (!is_file(dirname) && stat(dirname.c_str(), &sb) != 0) {
			std::cout << "Такой директории не существует" << std::endl;
			return 0;
		}
		std::cout << "Файл успешно перемещен по пути: " << path_to_new_file << std::endl;
		file.close();
		return std::rename(file_name.c_str(), path_to_new_file.c_str());
	}

	void execute_info(std::string file) {
		struct stat file_info;
		if (stat(file.c_str(), &file_info) == 0) {
			std::cout << "Размер файла: " << file_info.st_size << " байт\n";
			std::cout << "Права доступа файла: ";
			std::cout << ((file_info.st_mode & S_IRUSR) ? "r" : "-");
			std::cout << ((file_info.st_mode & S_IWUSR) ? "w" : "-");
			std::cout << ((file_info.st_mode & S_IXUSR) ? "x" : "-");
			std::cout << ((file_info.st_mode & S_IRGRP) ? "r" : "-");
			std::cout << ((file_info.st_mode & S_IWGRP) ? "w" : "-");
			std::cout << ((file_info.st_mode & S_IXGRP) ? "x" : "-");
			std::cout << ((file_info.st_mode & S_IROTH) ? "r" : "-");
			std::cout << ((file_info.st_mode & S_IWOTH) ? "w" : "-");
			std::cout << ((file_info.st_mode & S_IXOTH) ? "x" : "-") << std::endl;
			std::time_t modificationTime = file_info.st_mtime;
			std::cout << "Время последнего изменения: " << std::asctime(std::localtime(&modificationTime));
		}
		else {
			std::cout << "Файл не найден" << std::endl;
		}
	}

	void execute_chmod(std::string file, std::string perms) {
		if (is_mask(perms)) {
			change_permissions_mask(file.c_str(), atoi(perms.c_str()));
		}
		else {
			change_permissions_string(file.c_str(), perms.c_str());
		}
	}

	void change_permissions_string(const char* file_name, const char* perms)
	{
		std::filesystem::perms new_perms = std::filesystem::perms::owner_all | std::filesystem::perms::group_all | std::filesystem::perms::others_all;
		if (perms[0] == 'r') new_perms |= std::filesystem::perms::owner_read;
		if (perms[1] == 'w') new_perms |= std::filesystem::perms::owner_write;
		if (perms[2] == 'x') new_perms |= std::filesystem::perms::owner_exec;
		if (perms[3] == 'r') new_perms |= std::filesystem::perms::group_read;
		if (perms[4] == 'w') new_perms |= std::filesystem::perms::group_write;
		if (perms[5] == 'x') new_perms |= std::filesystem::perms::group_exec;
		if (perms[6] == 'r') new_perms |= std::filesystem::perms::others_read;
		if (perms[7] == 'w') new_perms |= std::filesystem::perms::others_write;
		if (perms[8] == 'x') new_perms |= std::filesystem::perms::others_exec;

		if (std::filesystem::status(file_name).permissions() != new_perms)
		{
			std::filesystem::permissions(file_name, new_perms);
			std::cout << "Права доступа для файла " << file_name << " успешно изменены!" << std::endl;
		}
		else
		{
			std::cout << "Права доступа для файла " << file_name << " уже имеют такое же значение" << std::endl;
		}
	}

	void change_permissions_mask(const char* file_name, int perms)
	{
		try
		{
			std::filesystem::permissions(file_name, std::filesystem::perms(perms));
			std::cout << "Права доступа файла были изменены успешно!" << std::endl;
		}
		catch (std::exception& e)
		{
			std::cout << "Файл не существует..." << std::endl;
		}
	}


	void delete_string_by_delimeter(std::string& str, char delimeter) {
		std::string::size_type pos{};   // позиция разделителя в строке
		pos = str.find_first_of(delimeter, pos);  // ищем первый символ, начиная с pos = 0, являющийся разделителем
		str.erase(0, pos + 1); // удаляем все что перед найденным символом
	}

	bool is_file(std::string path) {
		return path.find('.') != std::string::npos;
	}
};


std::vector<std::string> splitInput(std::string& str, char delim, std::vector<std::string>& out) {
	std::stringstream ss(str);
	std::string s;
	while (std::getline(ss, s, delim)) {
		out.push_back(s);
	}
	return out;

}


void inputSingleCommand(Command& command) {
	std::string raw_input, raw_command;
	std::getline(std::cin, raw_input);
	std::stringstream sstream(raw_input);
	std::vector<std::string> vector_command;
	while (std::getline(sstream, raw_command)) {
		vector_command = splitInput(raw_command, ' ', vector_command);
		command.execute(vector_command);
	}
}


void inputCommands() {
	bool ready_to_continue = true;
	Command command;
	while (ready_to_continue) {
		std::cout << "Введите команду для исполнения -> ";
		inputSingleCommand(command);
		ready_to_continue = command.is_default_name();
	}
}


input_types checkInput(int argc)
{
	return argc == 1 ? input_types::console : input_types::terminal;
}


void showTask() {
	std::cout << "Необходимо написать программу для работы с файлами, получающую информацию из командной строки или из консоли ввода\n" << std::endl;
}


void showActions() {
	std::cout << "Для получения команд для управления программой используйте команду help\n" << std::endl;
}


bool continueCheck()
/*
Функция ждет ввод пользователя.
Если пользователь вводит Y или y, функция возвращает 1.
Если пользователь вводит N или n, функция возвращает 0.
Если пользователь вводит не символы выше, проверка зацикливается и будет ждать символы выше.
*/
{
	bool flag = true;
	char symbol;
	std::cout << "Хотите продолжить(Y/y) или остановить(N/n) программу?" << '\n';
	while (!(std::cin >> symbol) || flag)
	{
		std::cin.clear();
		if ((symbol == 'N' || symbol == 'n') && (std::cin.peek() == '\n'))
		{
			return false;
		}
		else if ((symbol == 'Y' || symbol == 'y') && (std::cin.peek() == '\n'))
		{
			return true;
		}
		while (std::cin.get() != '\n');
		std::cout << "Ошибка :c Для работы доступны только продолжение (Y/y) и прекращение (N/n) работы.";
	}
}


void terminalAction()
{
	std::cout << "Terminal input" << std::endl;
	std::cout << "Ну да, я не сделал ввод с терминала. Вопросы?" << std::endl;
	std::cout << "В методичке было сказано или терминал, или консоль." << std::endl;
}


void consoleAction()
{
	bool ready_to_continue = true;
	showActions();
	while (ready_to_continue) {
		inputCommands();
		ready_to_continue = continueCheck();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
}


int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	showTask();
	input_types input = checkInput(argc);
	switch (input)
	{
	case input_types::terminal:
		terminalAction();
		break;
	case input_types::console:
		consoleAction();
		break;
	}
	return 0;
}

