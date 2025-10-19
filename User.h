#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <ctime>

enum class Role { BUYER, SELLER };

struct User {
    int id;
    std::string username;
    Role role;
    int bankAccountId; // -1 if none

    User() : id(-1), bankAccountId(-1) {}
    std::string serialize() const;
    static User deserialize(const std::string &line);
};

#endif
