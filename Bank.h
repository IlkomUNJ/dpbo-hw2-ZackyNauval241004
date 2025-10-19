#ifndef BANK_H
#define BANK_H

#include "BankAccount.h"
#include "Transaction.h"
#include <vector>
#include <map>
#include <string>

struct Bank {
    std::vector<BankAccount> accounts;
    std::vector<Transaction> transactions; // mirror of store transactions for payment records

    int nextAccountId;
    int nextTxId;

    Bank(): nextAccountId(1), nextTxId(1) {}
    BankAccount* createAccount(int ownerId);
    BankAccount* getAccountById(int id);
    BankAccount* getAccountByOwner(int ownerId);
    bool topup(int accountId, double amt);
    bool withdraw(int accountId, double amt);
    void recordTransaction(const Transaction &t); // store a bank record (simple)
    void load();
    void save();
    // queries
    std::vector<Transaction> transactionsLastWeek() const;
    std::vector<BankAccount*> dormantAccounts(); // no tx in 30 days
    std::vector<BankAccount*> listAccounts();
};

#endif
