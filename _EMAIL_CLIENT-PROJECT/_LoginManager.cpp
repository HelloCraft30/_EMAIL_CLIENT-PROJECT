#include "_LoginManager.h"

void GoTo(SHORT posX, SHORT posY)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD Position;
	Position.X = posX;
	Position.Y = posY;

	SetConsoleCursorPosition(hStdout, Position);
}

void Nocursortype(bool visible)
{
	CONSOLE_CURSOR_INFO Info;
	Info.bVisible = visible;
	Info.dwSize = 20;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Info);
}

string get_String(SHORT x, SHORT y, int len) {
	string str;
	while (true) {
		while (_kbhit()) {
			char key = _getch();
			switch (key) {
			case 8:
				if (str.size() != 0) str.pop_back();
				break;
			case 13:
				if (str.size() <= len) {
					GoTo(x, y);
					cout << str;
					for (int i = len - str.size(); i >= 0; i--) cout << " ";
				}
				else {
					GoTo(x, y);
					cout << "..";
					int n = str.size();
					for (int i = n - (len - 2); i <= n - 1; i++) {
						cout << str[i];
					}
				}
				return str;
				break;
			case 27:
				return "########################################";
			default:
				str.push_back(key);
				break;
			}

		}
		if (str.size() <= len) {
			GoTo(x, y);
			cout << str << (char)219;
			for (int i = len - str.size(); i >= 0; i--) cout << " ";
		}
		else {
			GoTo(x, y);
			cout << "..";
			int n = str.size();
			for (int i = n - (len - 2); i <= n - 1; i++) {
				cout << str[i];
			}
		}
	}
	return str;
}


bool isValidEmail(const std::string& email) {
	const std::regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
	return std::regex_match(email, pattern);
}

bool LoginManager::registerUser(const string& username, const string& password, const string& confirm) {
	if (!isValidEmail(username)) return false;
	if (password != confirm) return false;
	for (const auto& user : users) {
		if (user.username == username) {
			return false;
		}
	}
	User newUser;
	newUser.username = username;
	newUser.password = password;
	users.push_back(newUser);
	saveData();
	newMailBox(username);
	return true;
}

bool LoginManager::loginUser(const string& username, const string& password) {
	for (const auto& user : users) {
		if (user.username == username && user.password == password) {
			return true;
		}
	}
	return false;
}

void LoginManager::saveData() {
	ofstream file(USERFILE, ios::out | ios::trunc);
	if (file.is_open()) {
		for (const auto& user : users) {
			file << user.username << " " << user.password << endl;
		}
		file.close();
	}
}

void LoginManager::loadData() {
	ifstream file(USERFILE, ios::in);
	if (file.is_open()) {
		while (!file.eof()) {
			User user;
			file >> user.username >> user.password;
			users.push_back(user);
		}
		file.close();
	}
}

LoginManager::LoginManager() {
	loadData();
}

string LoginManager::run() {
	while (true) {
		system("cls");
		cout << "=============- EMAIL CLIENT -================\n";
		cout << "    1. Login\n";
		cout << "    2. Register\n";
		cout << "    0. Exit\n";
		cout << "    Your choice: ";
		char cmd = 0;
		cin >> cmd;
		switch (cmd) {
		case '0':
			exit(0);
		case '1': {
			system("cls");
			cout << "=================- LOGIN -===================\n";
			cout << "    Email: \n";
			cout << "    Password: \n";
			Nocursortype(0);
			string _acc, _pass;
			cin.ignore();
			_acc = get_String(11, 1, 30);
			GoTo(41, 1); cout << ' ';
			_pass = get_String(14, 2, 27);
			GoTo(41, 2); cout << ' ';
			Nocursortype(1);
			if (loginUser(_acc, _pass)) {
				GoTo(0, 3); cout << "    > SUCCED\n";
				cout << "=============================================\n";
				system("pause");
				return _acc;
			}
			else {
				GoTo(0, 3); cout << "    > Wrong email or password\n";
				cout << "=============================================\n";
				system("pause");
				break;
			}
			break;
		}
		case '2': {
			system("cls");
			cout << "================- REGISTER -=================\n";
			cout << "    Email: \n";
			cout << "    Password: \n";
			cout << "    Re-Password: \n";
			Nocursortype(0);
			string _acc, _pass, _repass;
			cin.ignore();

			_acc = get_String(11, 1, 30);
			GoTo(41, 1); cout << ' ';
			_pass = get_String(14, 2, 27);
			GoTo(41, 2); cout << ' ';
			_repass = get_String(17, 3, 24);
			GoTo(41, 3); cout << ' ';

			Nocursortype(1);

			if (registerUser(_acc, _pass, _repass)) {
				GoTo(0, 4); cout << "    > SUCCED\n";
				cout << "=============================================\n";
				system("pause");
			}
			else {
				GoTo(0, 4); cout << "    > Invalid email or wrong re-password.\n";
				cout << "=============================================\n";
				system("pause");
				system("cls");
			}
			break;
		}
		default:
			continue;
		}
	}
}