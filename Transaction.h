#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <ctime>

enum class TxStatus { PENDING, PAID, COMPLETED, CANCELED };

struct Transaction {
    int id;
    int buyerId;
    int sellerId;
    int itemId;
    int quantity;
    double amount;
    std::time_t timestamp;
    TxStatus status;

    Transaction(): id(-1), buyerId(-1), sellerId(-1), itemId(-1), quantity(0), amount(0), timestamp(0), status(TxStatus::PENDING) {}
    std::string serialize() const;
    static Transaction deserialize(const std::string &line);
    static std::string statusToChar(TxStatus s);
    static TxStatus charToStatus(char c);
};

#endif
