#pragma once
#include<fstream>
#include <direct.h>
#include<string>
#include<iostream>

#define DATAPATH "local_mail_data"

bool createFolder(const char* str);

bool newMailBox(const std::string& str);
