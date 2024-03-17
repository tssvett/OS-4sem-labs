#include <iostream>
#include <cmath>
#include <unistd.h>
#include <string>
#include <fstream>
#include <cstring>

int fd_in[2];
int fd_out[2];
pid_t pid;

struct HermiteData {
	int n;
	double x;
};

double hermitePolynomCalc(int n, double x)
{
	if (n == 0) return 1;
	else if (n == 1) return 2 * x;
	else return 2 * x * hermitePolynomCalc(n - 1, x) - 2 * (n - 1) * hermitePolynomCalc(n - 2, x);
}

int inputPolynomDegree()
/*
Ввод степпени полинома Эрмита. Степень всегда целое положительное число.
*/
{
	int polynom_degree;
	std::cout << "Введите степень полинома Эрмита (n): ";
	while (!(std::cin >> polynom_degree) || (std::cin.peek() != '\n') || (polynom_degree <= 0)) {
		std::cin.clear();
		while (std::cin.get() != '\n');
		std::cout << "Ошибка :c Введите корректное значение степени полинома Эрмита..." << '\n';
	}
	return polynom_degree;
}

double inputPolynomValue()
/*
Ввод x.
*/
{
	double polynom_degree;
	std::cout << "Введите значение x полинома Эрмита (x): ";
	while (!(std::cin >> polynom_degree) || (std::cin.peek() != '\n')) {
		std::cin.clear();
		while (std::cin.get() != '\n');
		std::cout << "Ошибка :c Введите корректное значение x полинома Эрмита..." << '\n';
	}
	return polynom_degree;
}

HermiteData getFileData(char* filename)
{
	std::ifstream in;
	in.open(filename, std::ios::binary);
	if (!in.is_open())
	{
		std::cerr << "Некорректное имя файла!" << std::endl;
		exit(1);
	}
	std::string str_data_degree;
	int data_degree;
	in >> str_data_degree;
	try
	{
		data_degree = std::stoi(str_data_degree);
	}
	catch (std::invalid_argument)
	{
		std::cerr << "Некорректные данные в файле....." << '\n';
		exit(1);
	}

	std::string str_data_value;
	double data_value;
	in >> str_data_value;
	in.close();
	try
	{
		data_value = std::stod(str_data_value);
		
	}
	catch (std::invalid_argument)
	{
		std::cerr << "Некорректные данные в файле....." << '\n';
		exit(1);
	}
	HermiteData data;
	data.n = data_degree;
	data.x = data_value;
	return data;
}

void outputToFile(char* filename, long double value) {
	std::ofstream out(filename, std::ios::binary);;
	out << value;
	out.close();
}

void server() {
	HermiteData data;
	read(fd_in[0], &data, sizeof(HermiteData));
	double result = hermitePolynomCalc(data.n, data.x);
	write(fd_out[1], &result, sizeof(double));
}

void client(char* input_file, char* output_file) {
	HermiteData data;
	if (input_file != NULL){
		data = getFileData(input_file);
	}
	else {
		data.n = inputPolynomDegree();
		data.x = inputPolynomValue();
	}
	write(fd_in[1], &data, sizeof(HermiteData));
	double result;
	read(fd_out[0], &result, sizeof(double));

	if (input_file != NULL) {
		outputToFile(output_file, result);
		std::cout << "Запись в файл проведена успешно!" << std::endl;
	}
	else {
		std::cout << "Результат вычисления полинома Эрмита для x = " << data.x << ": " << result << std::endl;
	}
}

void process(char* input_file, char* output_file) {
	pipe(fd_in);
	pipe(fd_out);
	pid = fork();
	if (pid < 0)
	{
		std::cerr << "Критическая ошибка! Форк не создан..." << std::endl;
		exit(1);
	}
	else if (pid > 0)
	{
		client(input_file, output_file);
	}
	else
	{
		server();
	}
	for (int i = 0; i < 2; ++i)
	{
		close(fd_in[i]);
		close(fd_out[i]);
	}
}

void help_command() {
	std::cout << "Чтобы вычислить значения полинома Эрмита при помощи консоли, запустите программу без дополнительных ключей" << std::endl;
	std::cout << "Чтобы вычислить значения полинома Эрмита при помощи файлового ввода, используйте ключи --file [INPUT FILENAME] [OUTPUT FILENAME]" << std::endl;

}

int main(int argc, char* argv[]) {
	setlocale(LC_ALL, 0);
	char* input_file = NULL;
	char* output_file = NULL;

	if (argc == 2 && !strcmp(argv[1], "--help")) {
		help_command();
		return 0;
	}
	else if (argc == 4 && !strcmp(argv[1], "--file")) {
		input_file = argv[2];
		output_file = argv[3];
	}
	else if (argc > 2 && argc != 4) {
		std::cout << "Некорректный ввод! Для получения помощи перезапустите программу с ключем --help" << std::endl;
		return 0;
	}
	process(input_file, output_file);
	return 0;
}
