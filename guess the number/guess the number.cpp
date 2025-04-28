#include<iostream>
#include <time.h>

using namespace std;

int main() {
	setlocale(LC_ALL, "RU");

	srand(time(NULL));

	int randomNum = 1 + rand() % 15;
	int attempts = 4;
	
	int userNum;
	int opinion;
	bool stop = false;

	cout << "У тебя есть 5 попыток чтобы угадать число от 1 до 15" << endl;
	cout << "Готов?" << endl;
	cout << "1 - Да" << endl;
	cout << "2 - Нет" << endl;

	cin >> opinion;


	
	if (opinion == 1) {
			cout << "Попытки: 5"<< endl;
		do {
			cout << "Введи число: " << endl;
			cin >> userNum;

			if (attempts == 0) {
				cout << "Попытки кончилиcь!" << endl;
				cout << "Было загадано число: " << randomNum << endl;
				cout << "Нажмите Enter для выхода...";
				cin.ignore();
				cin.get();

				return 1;
			}
			else {
				if (userNum < 1 || userNum > 15) {
					cout << "Число не входит в диапозон!" << endl;
					
				}
				else if (userNum != randomNum) {
					cout << "Неверное число!" << endl;
					cout << "Попытки: " <<attempts<< endl;
					attempts--;
				}
				else {
					cout << "Ты угадал!" << endl;
					stop = true;
				}
			}

			
	} while (!stop);

	}
	else {
		cout << "До свидания!";
	}
	cout << "Нажмите Enter для выхода...";

	cin.ignore();
	cin.get(); 

	return 0;
	
}