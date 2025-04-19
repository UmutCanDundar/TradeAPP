#ifndef SYSTEM_H
#define SYSTEM_H

#include "User.h"
#include "auxiliary.h"

#include <unordered_set>
#include <iterator>

class System {

    friend QDebug &operator<< (QDebug& debug, const System &system);

private:
    std::unordered_set<std::shared_ptr<User>> users;

public:
    System();

    bool Create_User(const std::string &username, const std::string &password);
    std::unordered_set<std::shared_ptr<User>>::iterator Current_User(const std::string &username_toCheck, const std::string &password_toCheck);
    bool Check_User (const std::string &username_toCheck, const std::string &password_toCheck);
    bool Add_Account();   
};
#endif
