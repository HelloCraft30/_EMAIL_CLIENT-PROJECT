#pragma once

#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<regex>
#include<conio.h>
#include<Windows.h>
#include "_Model.h"

using namespace std;

#define USERFILE "users.txt"

//check if valid email
bool isValidEmail(const std::string& email);

struct User {
	string username;
	string password;
};

class LoginManager {
public:
	bool registerUser(const string& username, const string& password, const string& confirm);

	bool loginUser(const string& username, const string& password);

	string run();

private:
	vector<User> users;

	void saveData();

	void loadData();

public:
	LoginManager();
};