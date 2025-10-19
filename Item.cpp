#include "Item.h"
#include <sstream>

std::string Item::serialize() const {
    std::ostringstream o;
    o << id << ',' << sellerId << ',' << name << ',' << stock << ',' << price << ',' << soldCount;
    return o.str();
}

Item Item::deserialize(const std::string &line) {
    Item it;
    std::istringstream iss(line);
    std::string token;
    std::getline(iss, token, ','); it.id = std::stoi(token);
    std::getline(iss, token, ','); it.sellerId = std::stoi(token);
    std::getline(iss, it.name, ',');
    std::getline(iss, token, ','); it.stock = std::stoi(token);
    std::getline(iss, token, ','); it.price = std::stod(token);
    std::getline(iss, token, ','); it.soldCount = std::stoi(token);
    return it;
}
