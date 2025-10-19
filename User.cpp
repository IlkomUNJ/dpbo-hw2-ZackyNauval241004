#include "User.h"
#include <sstream>

std::string User::serialize() const {
    std::ostringstream oss;
    oss << id << ',' << username << ',' << (role==Role::SELLER? 'S':'B') << ',' << bankAccountId;
    return oss.str();
}

User User::deserialize(const std::string &line) {
    User u;
    std::istringstream iss(line);
    std::string token;
    std::getline(iss, token, ','); u.id = std::stoi(token);
    std::getline(iss, u.username, ',');
    std::getline(iss, token, ','); u.role = (token[0]=='S'? Role::SELLER: Role::BUYER);
    std::getline(iss, token, ','); u.bankAccountId = std::stoi(token);
    return u;
}
