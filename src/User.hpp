#pragma once
#include <string>


class User {
public:
    std::string username;
    std::string sessionid;

    User(const std::string& uname, const std::string& sessionid) {
        this->username = uname;
        this->sessionid = sessionid;
    }
};