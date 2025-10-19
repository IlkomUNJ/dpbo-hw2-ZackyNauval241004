#ifndef BANKACCOUNT_H
#define BANKACCOUNT_H

#include <string>
#include <ctime>

struct BankAccount {
    int id;
    int ownerUserId;
    double balance;
    std::time_t lastActivity; // timestamp of last tx

    BankAccount(): id(-1), ownerUserId(-1), balance(0.0), lastActivity(0) {}
    std::string serialize() const;
    static BankAccount deserialize(const std::string &line);
};

#endif
