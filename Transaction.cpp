#include "Transaction.h"
#include <sstream>

std::string Transaction::statusToChar(TxStatus s) {
    switch(s){
        case TxStatus::PENDING: return "P";
        case TxStatus::PAID: return "A";
        case TxStatus::COMPLETED: return "C";
        case TxStatus::CANCELED: return "X";
    }
    return "P";
}

TxStatus Transaction::charToStatus(char c) {
    if(c=='A') return TxStatus::PAID;
    if(c=='C') return TxStatus::COMPLETED;
    if(c=='X') return TxStatus::CANCELED;
    return TxStatus::PENDING;
}

std::string Transaction::serialize() const {
    std::ostringstream o;
    o << id << ',' << buyerId << ',' << sellerId << ',' << itemId << ',' << quantity << ',' << amount << ',' << timestamp << ',' << statusToChar(status);
    return o.str();
}

Transaction Transaction::deserialize(const std::string &line) {
    Transaction t;
    std::istringstream iss(line);
    std::string token;
    std::getline(iss, token, ','); t.id = std::stoi(token);
    std::getline(iss, token, ','); t.buyerId = std::stoi(token);
    std::getline(iss, token, ','); t.sellerId = std::stoi(token);
    std::getline(iss, token, ','); t.itemId = std::stoi(token);
    std::getline(iss, token, ','); t.quantity = std::stoi(token);
    std::getline(iss, token, ','); t.amount = std::stod(token);
    std::getline(iss, token, ','); t.timestamp = (std::time_t)std::stoll(token);
    std::getline(iss, token, ','); t.status = charToStatus(token[0]);
    return t;
}
