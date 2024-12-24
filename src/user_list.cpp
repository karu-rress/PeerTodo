/**
 *
 * user_list.cpp
 *
 * List of users
 *
 * Author: Sunwoo Na
 *
 */

#include <algorithm>
#include <sstream>

#include "../include/user_list.h"

using namespace std;

namespace p2d {
bool user_list::add(const user &u) {
    return users.insert(u).second;
}

[[nodiscard]] const user &user_list::operator[](string_view id) const {
    return *find(id);
}

[[nodiscard]] set<user>::const_iterator user_list::find(string_view id) const {
    return ranges::find_if(users, [id](const user &u) {
        return u.get_id() == id;
    });
}

bool user_list::remove(string_view id) {
    if (auto user = find(id); user != end(users)) {
        users.erase(user);
        return true;
    }
    return false;
}

[[nodiscard]] bool user_list::contains(string_view id) const {
    return find(id) != end(users);
}

[[nodiscard]] const set<user> &user_list::get_users() const {
    return users;
}
}