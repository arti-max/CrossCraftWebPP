#pragma once
#include <string>


class User {
public:
    std::string username;
    std::string sessionid;

    User(std::string& uname, std::string& sessionid) {
        this->username = uname;
        this->sessionid = sessionid;
    }
};