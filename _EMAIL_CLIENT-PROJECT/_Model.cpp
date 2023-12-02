#include "_Model.h"
bool createFolder(const char* str) {
    return !_mkdir(str);
}

bool newMailBox(const std::string& str) {
    std::string tmp = DATAPATH;
    tmp += "\\" + str;
    createFolder(tmp.c_str());
    //default folder mail
    //1. Inbox
    std::string tmp2 = tmp + "\\" + "Inbox";
    createFolder(tmp2.c_str());
    //2. Project
    tmp2 = tmp + "\\" + "Project";
    createFolder(tmp2.c_str());
    //3. Important
    tmp2 = tmp + "\\" + "Important";
    createFolder(tmp2.c_str());
    //4. Work
    tmp2 = tmp + "\\" + "Work";
    createFolder(tmp2.c_str());
    //5. Spam
    tmp2 = tmp + "\\" + "Spam";
    createFolder(tmp2.c_str());
    return true;
}