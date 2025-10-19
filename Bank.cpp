#include "Bank.h"
#include <fstream>
#include <filesystem>
#include <chrono>

using namespace std::chrono;
namespace fs = std::filesystem;

void Bank::load() {
    accounts.clear(); transactions.clear();
    if(fs::exists("bank_accounts.txt")) {
        std::ifstream f("bank_accounts.txt");
        std::string line;
        while(std::getline(f,line)) if(!line.empty()) accounts.push_back(BankAccount::deserialize(line));
        if(!accounts.empty()) nextAccountId = accounts.back().id + 1;
    }
    if(fs::exists("bank_transactions.txt")) {
        std::ifstream f("bank_transactions.txt");
        std::string line;
        while(std::getline(f,line)) if(!line.empty()) transactions.push_back(Transaction::deserialize(line));
        if(!transactions.empty()) nextTxId = transactions.back().id + 1;
    }
}

void Bank::save() {
    std::ofstream fa("bank_accounts.txt");
    for(auto &a: accounts) fa << a.serialize() << '\n';
    std::ofstream ft("bank_transactions.txt");
    for(auto &t: transactions) ft << t.serialize() << '\n';
}

BankAccount* Bank::createAccount(int ownerId) {
    BankAccount acc;
    acc.id = nextAccountId++;
    acc.ownerUserId = ownerId;
    acc.balance = 0;
    acc.lastActivity = std::time(nullptr);
    accounts.push_back(acc);
    save();
    return &accounts.back();
}

BankAccount* Bank::getAccountById(int id) {
    for(auto &a: accounts) if(a.id==id) return &a;
    return nullptr;
}

BankAccount* Bank::getAccountByOwner(int ownerId) {
    for(auto &a: accounts) if(a.ownerUserId==ownerId) return &a;
    return nullptr;
}

bool Bank::topup(int accountId, double amt) {
    auto *a = getAccountById(accountId);
    if(!a) return false;
    a->balance += amt;
    a->lastActivity = std::time(nullptr);
    save();
    return true;
}

bool Bank::withdraw(int accountId, double amt) {
    auto *a = getAccountById(accountId);
    if(!a) return false;
    if(a->balance < amt) return false;
    a->balance -= amt;
    a->lastActivity = std::time(nullptr);
    save();
    return true;
}

void Bank::recordTransaction(const Transaction &t) {
    Transaction copy = t;
    copy.id = nextTxId++;
    transactions.push_back(copy);
    // update lastActivity for involved accounts if present
    auto now = std::time(nullptr);
    for(auto &a: accounts) {
        if(a.ownerUserId==t.buyerId || a.ownerUserId==t.sellerId) a.lastActivity = now;
    }
    save();
}

std::vector<Transaction> Bank::transactionsLastWeek() const {
    std::vector<Transaction> out;
    auto now = std::time(nullptr);
    for(auto &t: transactions) {
        if(difftime(now, t.timestamp) <= 7*24*3600) out.push_back(t);
    }
    return out;
}

std::vector<BankAccount*> Bank::dormantAccounts() {
    std::vector<BankAccount*> out;
    auto now = std::time(nullptr);
    for(auto &a: accounts) {
        if(difftime(now, a.lastActivity) >= 30*24*3600) out.push_back(&a);
    }
    return out;
}

std::vector<BankAccount*> Bank::listAccounts() {
    std::vector<BankAccount*> out;
    for(auto &a: accounts) out.push_back(&a);
    return out;
}
