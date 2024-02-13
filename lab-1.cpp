#include <stdio.h>
#include <iostream>
/*
Для запуска на виртуальной машине Linux необходимо ввести следующие команды:

cd путь/к/рабочей/директории/
g++ -o program lab-1.cpp
./program

После ввода комманд запустится скомпилированная программа :)
*/

void printTask()
{
    std::cout << "Вариант 10. Написать функцию вычисления суммы элементов заданного одномерного массива.\n";  
}

int inputArraySize()
/*
Ввод длины массива. Длина массива всегда целое положительное число.
*/
{
    int array_size;
    std::cout << "Введите длину массива: ";
	while (!(std::cin >> array_size) || (std::cin.peek() != '\n') || (array_size <= 0)) {
		std::cin.clear();
		while (std::cin.get() != '\n');
		std::cout << "Ошибка :c Введите корректное значение длины массива..." << '\n';
	}
	return array_size;
}

double* inputArray(int array_size)
/*
Ввод элементов массива. Числа могут быть положительные и отрицательные, целые и с плавающей точкой.
*/
{
    double* array = new double[array_size];
	for (int i = 0; i < array_size; i++) {
		std::cout << "Введите элемент массива: ";
		while (!(std::cin >> array[i]) || (std::cin.peek() != '\n')) {
			std::cin.clear();
			while (std::cin.get() != '\n');
		std::cout << "Ошибка :c Введите корректный элемент массива..." << '\n';
		}
	}
    return array;
}

void printArray(double *array, int array_size)
/*
Вывод массива в консоль.
*/
{
    std::cout << "Введенный массив: { ";  
    for (int i = 0; i < array_size; i++)
    {
        std::cout << array[i] << ' ';
    }
    std::cout << "}" << '\n';  

}

double calculateArraySum(double *array, int array_size)
/*
Подсчет суммы элементов внутри массива.
*/
{
    double array_sum = 0;
    for (int i = 0; i < array_size; i++)
    {
        array_sum += array[i];
    }
    return array_sum;
}



void printArraySum(double array_sum)
{
    std::cout << "Сумма элементов для введенного массива: " << array_sum << '\n'; 
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

void makeOneStep(bool &ready_to_continue)
/*
Выполняет одну итерацию программы.

Вводим длину массива
Вводим элементы массива
Считаем сумму элементов
Выводим сумму элементов
Удаляем массив
Вводим символ для продолжения работы
*/
{
    int array_size = inputArraySize();
    double* array = inputArray(array_size);
    printArray(array, array_size);
    double array_sum = calculateArraySum(array, array_size);
    printArraySum(array_sum);  
    delete[] array;
    ready_to_continue = continueCheck();
}


int main(int argc, char* argv[])
{
    printTask();
    bool ready_to_continue = true;
    while(ready_to_continue)
    {
        makeOneStep(ready_to_continue);
    }
    return 0; 
}  
