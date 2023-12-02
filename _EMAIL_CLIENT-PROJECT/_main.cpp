#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include<sstream>
#include<winsock.h>
#include "_LoginManager.h"

#pragma comment(lib, "ws2_32.lib")

#define SMTP_PORT 2225
#define POP3_PORT 3335
#define HOST "127.0.0.1"

struct MAIL {
	string mailFrom;
	vector<string> sendTo;
	vector<string> sendCC;
	vector<string> sendBCC;
	string subject;
	vector<string> content;
	vector<string> pathFiles;
};

vector<string> getMails(const string& a);

string trimSpaces(const string& input);

void writeMail(MAIL& mail);

void doSMTP(SOCKET& socket, const MAIL& mail);

void run(SOCKET& smtp, SOCKET& pop3, const string& localEmail) {
	while (true) {
		cout << "--------------- EMAIL CLIENT ---------------\n\n";
		cout << "      Hello <" << localEmail << ">\n\n";
		cout << "MENU:\n";
		cout << "1. Send an email\n";
		cout << "2. Check the list of received emails\n";
		cout << "3. Quit\n";
		cout << "Your choice: ";
		char cmd = 0;
		cin >> cmd;
		switch (cmd) {
		case '3':
			exit(0);
		case '1': {
			MAIL mail;
			mail.mailFrom = localEmail;
			writeMail(mail);
			doSMTP(smtp, mail);
			break;
		}
		case '4': {//update mails to inbox
			char buffer[4096]{ 0 };
			string command = "USER " + localEmail + "\r\n";
			recv(pop3, buffer, sizeof(buffer), 0);
			memset(buffer, 0, sizeof(buffer));
			send(pop3, command.c_str(), command.size(), 0);
			recv(pop3, buffer, sizeof(buffer), 0);
			memset(buffer, 0, sizeof(buffer));
			command = "STAT\r\n";
			send(pop3, command.c_str(), command.size(), 0);
			recv(pop3, buffer, sizeof(buffer), 0);
			stringstream ss{ buffer };
			string trash;
			ss >> trash;
			int count = 0;
			ss >> count;
			int i = 1;
			while (count--) {
				string num = to_string(i);
				command = "RETR " + num + "\r\n";
				send(pop3, command.c_str(), command.size(), 0);
				recv(pop3, buffer, sizeof(buffer), 0);

				string a;
				for (int j = 0; j < sizeof(buffer); j++) a.push_back(buffer[j]);

				cout << a;

				string path = "local_mail_data\\" + localEmail + "\\" + "Inbox\\"  + ".txt";
				fstream fileOpen(path.c_str(), ios::out | ios::trunc);

				for (int j = 0; j < a.size(); j++) {
					fileOpen << a[j];
				}
				fileOpen.close();
			}

			break;
		}
		}
	}
}

using namespace std;

int main() {
	// Initialize Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cout << "Failed to initialize Winsock.\n";
		return -1;
	}

	// Create socket
	SOCKET smtpSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (smtpSocket == INVALID_SOCKET) {
		std::cerr << "Failed to create client socket.\n";
		WSACleanup();
		return -1;
	}

	SOCKET pop3Socket = socket(AF_INET, SOCK_STREAM, 0);

	// Connect to SMTP-server
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SMTP_PORT); // Port number
	serverAddress.sin_addr.s_addr = inet_addr(HOST); // IP address of the server

	// Connect to POP3-server
	sockaddr_in serverAddress2;
	serverAddress2.sin_family = AF_INET;
	serverAddress2.sin_port = htons(POP3_PORT); // Port number
	serverAddress2.sin_addr.s_addr = inet_addr(HOST); // IP address of the server

	if (connect(smtpSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
		cout << "Failed to connect to server.\n";
		closesocket(smtpSocket);
		WSACleanup();
		return -1;
	}

	if (connect(pop3Socket, (struct sockaddr*)&serverAddress2, sizeof(serverAddress2)) == SOCKET_ERROR) {
		cout << "Failed to connect to server.\n";
		closesocket(pop3Socket);
		WSACleanup();
		return -1;
	}

	cout << "    Connected to server.\n";
	system("pause");

	//login
	LoginManager loginManager;
	string localEmail = loginManager.run();
	system("cls");

	run(smtpSocket, pop3Socket, localEmail);
	return 0;
}

vector<string> getMails(const string& a) {
	stringstream ss{ a };
	vector<string> rs;
	string tmp;
	while (getline(ss, tmp, ',')) {
		tmp = trimSpaces(tmp);
		rs.push_back(tmp);
	}
	return rs;
}

string trimSpaces(const string& input) {
	size_t start = input.find_first_not_of(" \t\n\r\f\v");
	if (start == string::npos) {
		return "";
	}
	size_t end = input.find_last_not_of(" \t\n\r\f\v");
	return input.substr(start, end - start + 1);
}

void writeMail(MAIL& mail) {
	cin.ignore();
	cout << "This is the email content: (press ENTER to skip if not filled).\n";
	cout << "To: ";
	string tmp;
	tmp = trimSpaces(tmp);
	getline(cin, tmp);
	if (tmp == "\n") cin.ignore();
	else mail.sendTo = getMails(tmp);
	cout << "CC: ";
	getline(cin, tmp);
	if (tmp == "\n") cin.ignore();
	else mail.sendCC = getMails(tmp);
	cout << "BCC: ";
	getline(cin, tmp);
	if (tmp == "\n") cin.ignore();
	else mail.sendBCC = getMails(tmp);
	cout << "Subject: ";
	getline(cin, mail.subject);
	cout << "\"END\" to cancel typing content.\n";
	while (getline(cin, tmp)) {
		if (tmp == "END") break;
		mail.content.push_back(tmp);
	}
	int athF = 0;
	int numF = 0;
	cout << "Attach file [1: yes, 0: no]: ";
	cin >> athF;
	if (athF == 1) {
		cout << "Number of files: ";
		cin >> numF;
		int i = 0;
		while (numF--) {
			cout << "Path " << i << ": ";
			getline(cin, tmp);
			mail.pathFiles.push_back(tmp);
		}
	}
	else if (athF != 0) {
		cout << "Invalid input!.\nDefault: No attach file.\n";
	}
	cin.clear();
}

void doSMTP(SOCKET& socket, const MAIL& mail) {
	string cmd = "EHLO ";
	cmd += HOST; cmd += "\r\n";
	send(socket, cmd.c_str(), cmd.size(), 0);
	cmd = "MAIL FROM: <" + mail.mailFrom; cmd += ">\r\n";
	send(socket, cmd.c_str(), cmd.size(), 0);
	for (const auto& a : mail.sendTo) {
		cmd = "RCPT TO: <" + a;
		cmd += ">\r\n";
		send(socket, cmd.c_str(), cmd.size(), 0);
	}
	for (const auto& a : mail.sendCC) {
		cmd = "RCPT TO: <" + a;
		cmd += ">\r\n";
		send(socket, cmd.c_str(), cmd.size(), 0);
	}
	for (const auto& a : mail.sendBCC) {
		cmd = "RCPT TO: <" + a;
		cmd += ">\r\n";
		send(socket, cmd.c_str(), cmd.size(), 0);
	}
	send(socket, "DATA\r\n", sizeof("DATA\r\n"), 0);

	cmd = "Subject: " + mail.subject + "\r\n";
	send(socket, cmd.c_str(), cmd.size(), 0);

	cmd = "From: " + mail.mailFrom + "\r\n";
	send(socket, cmd.c_str(), cmd.size(), 0);

	cmd = "To: ";
	for (const auto& a : mail.sendTo) cmd += a + ", "; {
		if (!(cmd == "To: ")) cmd.pop_back(); cmd.pop_back();
	}
	cmd += "\r\n";
	send(socket, cmd.c_str(), cmd.size(), 0);

	cmd = "Cc: ";
	for (const auto& a : mail.sendCC) cmd += a + ", ";
	if (!(cmd == "Cc: ")) {
		cmd.pop_back(); cmd.pop_back();
	}
	cmd += "\r\n";
	send(socket, cmd.c_str(), cmd.size(), 0);

	for (const auto& a : mail.content) {
		cmd = a + "\r\n";
		send(socket, cmd.c_str(), cmd.size(), 0);
	}
	send(socket, ".\r\n", sizeof(".\r\n"), 0);
}