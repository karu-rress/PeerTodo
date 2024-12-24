/**
 *
 * user_list.h
 *
 * List of users
 *
 * Author: Sunwoo Na
 *
 */

#ifndef _USER_LIST_H_
#define _USER_LIST_H_

#include <set>
#include <string_view>

#include <boost/serialization/access.hpp>
#include <boost/serialization/set.hpp>

#include "user.h"

namespace p2d {
class user_list {
    friend class boost::serialization::access;

public:
    user_list() = default;

    // Remove copy semantics
    user_list(const user_list &other) = delete;
    user_list &operator=(const user_list &other) = delete;

    bool add(const user &u);

    [[nodiscard]] const user &operator[](std::string_view id) const;
    [[nodiscard]] std::set<user>::const_iterator find(std::string_view id) const;

    bool remove(std::string_view id);
    [[nodiscard]] bool contains(std::string_view id) const;
    [[nodiscard]] const std::set<user> &get_users() const;

private:
    std::set<user> users;

    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & users;
    }
};
}

#endif