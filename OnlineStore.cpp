#include "OnlineStore.h"
#include <fstream>
#include <filesystem>
#include <sstream>
#include <algorithm>
#include <ctime>

namespace fs = std::filesystem;

void OnlineStore::load() {
    users.clear(); items.clear(); transactions.clear();
    bank.load();
    if(fs::exists("users.txt")) {
        std::ifstream f("users.txt"); std::string line;
        while(std::getline(f,line)) if(!line.empty()) users.push_back(User::deserialize(line));
        if(!users.empty()) nextUserId = users.back().id + 1;
    }
    if(fs::exists("items.txt")) {
        std::ifstream f("items.txt"); std::string line;
        while(std::getline(f,line)) if(!line.empty()) items.push_back(Item::deserialize(line));
        if(!items.empty()) nextItemId = items.back().id + 1;
    }
    if(fs::exists("transactions.txt")) {
        std::ifstream f("transactions.txt"); std::string line;
        while(std::getline(f,line)) if(!line.empty()) transactions.push_back(Transaction::deserialize(line));
        if(!transactions.empty()) nextTxId = transactions.back().id + 1;
    }
}

void OnlineStore::save() {
    std::ofstream fu("users.txt");
    for(auto &u: users) fu << u.serialize() << '\n';
    std::ofstream fi("items.txt");
    for(auto &i: items) fi << i.serialize() << '\n';
    std::ofstream ft("transactions.txt");
    for(auto &t: transactions) ft << t.serialize() << '\n';
    bank.save();
}

User* OnlineStore::registerUser(const std::string &username, Role role) {
    if(findUserByName(username)) return nullptr;
    User u; u.id = nextUserId++; u.username = username; u.role = role; u.bankAccountId = -1;
    users.push_back(u);
    save();
    return &users.back();
}

User* OnlineStore::findUserByName(const std::string &username) {
    for(auto &u: users) if(u.username==username) return &u;
    return nullptr;
}
User* OnlineStore::findUserById(int id) {
    for(auto &u: users) if(u.id==id) return &u;
    return nullptr;
}

Item* OnlineStore::addItem(int sellerId, const std::string &name, int stock, double price) {
    Item it; it.id = nextItemId++; it.sellerId = sellerId; it.name = name; it.stock = stock; it.price = price; it.soldCount=0;
    items.push_back(it);
    save();
    return &items.back();
}
Item* OnlineStore::findItemById(int id) {
    for(auto &it: items) if(it.id==id) return &it;
    return nullptr;
}

Transaction* OnlineStore::createOrder(int buyerId, int itemId, int qty) {
    Item* it = findItemById(itemId);
    if(!it) return nullptr;
    if(it->stock < qty) return nullptr;
    Transaction t;
    t.id = nextTxId++; t.buyerId = buyerId; t.sellerId = it->sellerId; t.itemId = itemId; t.quantity = qty;
    t.amount = it->price * qty;
    t.timestamp = std::time(nullptr);
    t.status = TxStatus::PENDING;
    transactions.push_back(t);
    save();
    return &transactions.back();
}

bool OnlineStore::payOrder(int txId) {
    for(auto &t: transactions) {
        if(t.id==txId) {
            if(t.status != TxStatus::PENDING) return false;
            // check buyer bank account and balance
            User* buyer = findUserById(t.buyerId);
            User* seller = findUserById(t.sellerId);
            if(!buyer || !seller) return false;
            auto *accBuyer = bank.getAccountByOwner(buyer->id);
            auto *accSeller = bank.getAccountByOwner(seller->id);
            if(!accBuyer || !accSeller) return false;
            if(accBuyer->balance < t.amount) return false;
            // transfer
            accBuyer->balance -= t.amount;
            accSeller->balance += t.amount;
            accBuyer->lastActivity = std::time(nullptr);
            accSeller->lastActivity = std::time(nullptr);
            t.status = TxStatus::PAID;
            bank.recordTransaction(t);
            save();
            return true;
        }
    }
    return false;
}

bool OnlineStore::completeOrder(int txId) {
    for(auto &t: transactions) {
        if(t.id==txId) {
            if(t.status != TxStatus::PAID) return false;
            t.status = TxStatus::COMPLETED;
            // reduce stock and increment soldCount
            Item* it = findItemById(t.itemId);
            if(it) { it->stock -= t.quantity; it->soldCount += t.quantity; }
            save();
            return true;
        }
    }
    return false;
}

std::vector<Transaction> OnlineStore::listTransactionsLastKDays(int k) const {
    std::vector<Transaction> out;
    auto now = std::time(nullptr);
    for(auto &t: transactions) {
        if(difftime(now, t.timestamp) <= k*24*3600) out.push_back(t);
    }
    return out;
}

std::vector<Transaction> OnlineStore::listPaidNotCompleted() const {
    std::vector<Transaction> out;
    for(auto &t: transactions) if(t.status==TxStatus::PAID) out.push_back(t);
    return out;
}

std::vector<int> OnlineStore::topMItems(int m) const {
    std::vector<std::pair<int,int>> v; // itemId, soldCount
    for(auto &it: items) v.push_back({it.id, it.soldCount});
    std::sort(v.begin(), v.end(), [](auto &a, auto &b){ return a.second > b.second; });
    std::vector<int> out;
    for(int i=0;i<(int)std::min((int)v.size(), m);++i) out.push_back(v[i].first);
    return out;
}

std::vector<int> OnlineStore::mostActiveBuyersPerDay(int topN) const {
    std::map<int,int> counts;
    // count number of transactions today per buyer
    std::time_t now = std::time(nullptr);
    std::tm *tmNow = std::localtime(&now);
    for(auto &t: transactions) {
        std::tm *tmT = std::localtime(&t.timestamp);
        if(tmNow->tm_year==tmT->tm_year && tmNow->tm_yday==tmT->tm_yday) counts[t.buyerId]++;
    }
    std::vector<std::pair<int,int>> v;
    for(auto &p: counts) v.push_back(p);
    std::sort(v.begin(), v.end(), [](auto &a, auto &b){ return a.second > b.second; });
    std::vector<int> out;
    for(int i=0;i<(int)std::min((int)v.size(), topN); ++i) out.push_back(v[i].first);
    return out;
}

std::vector<int> OnlineStore::mostActiveSellersPerDay(int topN) const {
    std::map<int,int> counts;
    std::time_t now = std::time(nullptr);
    std::tm *tmNow = std::localtime(&now);
    for(auto &t: transactions) {
        std::tm *tmT = std::localtime(&t.timestamp);
        if(tmNow->tm_year==tmT->tm_year && tmNow->tm_yday==tmT->tm_yday) counts[t.sellerId]++;
    }
    std::vector<std::pair<int,int>> v;
    for(auto &p: counts) v.push_back(p);
    std::sort(v.begin(), v.end(), [](auto &a, auto &b){ return a.second > b.second; });
    std::vector<int> out;
    for(int i=0;i<(int)std::min((int)v.size(), topN); ++i) out.push_back(v[i].first);
    return out;
}
