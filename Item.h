#ifndef ITEM_H
#define ITEM_H

#include <string>

struct Item {
    int id;
    int sellerId;
    std::string name;
    int stock;
    double price;
    int soldCount;

    Item(): id(-1), sellerId(-1), stock(0), price(0), soldCount(0) {}
    std::string serialize() const;
    static Item deserialize(const std::string &line);
};

#endif
