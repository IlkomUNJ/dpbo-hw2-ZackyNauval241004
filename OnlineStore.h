#ifndef ONLINESTORE_H
#define ONLINESTORE_H

#include "User.h"
#include "Item.h"
#include "Transaction.h"
#include "Bank.h"
#include <vector>
#include <map>
#include <string>

struct OnlineStore {
    std::vector<User> users;
    std::vector<Item> items;
    std::vector<Transaction> transactions;

    Bank bank;

    int nextUserId;
    int nextItemId;
    int nextTxId;

    OnlineStore(): nextUserId(1), nextItemId(1), nextTxId(1) {}

    // persistence
    void load();
    void save();

    // users
    User* registerUser(const std::string &username, Role role);
    User* findUserByName(const std::string &username);
    User* findUserById(int id);

    // items
    Item* addItem(int sellerId, const std::string &name, int stock, double price);
    Item* findItemById(int id);

    // transactions / orders
    Transaction* createOrder(int buyerId, int itemId, int qty);
    bool payOrder(int txId); // interact with bank
    bool completeOrder(int txId);
    std::vector<Transaction> listTransactionsLastKDays(int k) const;
    std::vector<Transaction> listPaidNotCompleted() const;

    // stats
    std::vector<int> topMItems(int m) const; // return itemIds
    std::vector<int> mostActiveBuyersPerDay(int topN) const; // userIds
    std::vector<int> mostActiveSellersPerDay(int topN) const;
};

#endif
