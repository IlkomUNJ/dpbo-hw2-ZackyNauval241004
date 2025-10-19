#include "BankAccount.h"
#include <sstream>

std::string BankAccount::serialize() const {
    std::ostringstream o;
    o << id << ',' << ownerUserId << ',' << balance << ',' << lastActivity;
    return o.str();
}

BankAccount BankAccount::deserialize(const std::string &line) {
    BankAccount b;
    std::istringstream iss(line);
    std::string token;
    std::getline(iss, token, ','); b.id = std::stoi(token);
    std::getline(iss, token, ','); b.ownerUserId = std::stoi(token);
    std::getline(iss, token, ','); b.balance = std::stod(token);
    std::getline(iss, token, ','); b.lastActivity = (std::time_t)std::stoll(token);
    return b;
}
