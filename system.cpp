#include "system.h"
#include <utility>


System::System() {}

bool System::Create_User(const std::string &username, const std::string &password)
{
    if(!Check_User(username, password)) {
        this->users.insert(std::move(std::make_shared<User>(username, password)));
        return true;
    } else {
        return false;
    }

}

QDebug &operator<< (QDebug& debug, const System &system)
{
    for (auto& users : system.users)
        debug.nospace() << users;

    return debug;
}

std::unordered_set<std::shared_ptr<User>>::iterator System::Current_User(const std::string &username_toCheck,
                                                                         const std::string &password_toCheck)
{
    auto User_it = std::find_if(users.begin(), users.end(), [&username_toCheck, &password_toCheck]
                                                            (const std::shared_ptr<User> &User) {
        return (User->username == username_toCheck && User->password == password_toCheck);
    });

    return User_it;
}

bool System::Check_User(const std::string &username_toCheck, const std::string &password_toCheck)
{
    return Current_User(username_toCheck, password_toCheck) != users.end();
}
