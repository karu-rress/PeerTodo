/**
 *
 * user_list.h
 *
 * A user in p2d
 *
 * Author: Sunwoo Na
 *
 */

#ifndef _USER_H_
#define _USER_H_

#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>

#include <boost/serialization/access.hpp>

namespace p2d {
// Password class
class password {
public:
    password() = default;
    password(std::string_view pw);
    password(password &&other) noexcept = default;

    // Delete copy semantics
    password(const password &other) = delete;
    password &operator=(const password &other) = delete;

    // Operators
    password &operator=(password &&other) noexcept = default;
    bool operator==(const password &other) const = default;
    bool operator==(std::string_view other) const;

private:
    std::string create_hash(std::string_view pw);
    std::string hash;
};

// Stream operator
std::istream &operator>>(std::istream &is, password &pw);

// User class
class user {
    friend class boost::serialization::access;

public:
    user() = default; // only for serialization
    user(std::string_view name,
        std::string_view email,
        std::string_view id,
        password &&pw);
    user(const user &other);
    user(user &&other) noexcept = default;

    // Getters
    std::string get_name() const;
    std::string get_email() const;
    std::string get_id() const;

    // Operators
    user &operator=(const user &other);
    user &operator=(user &&other) noexcept = default;
    bool operator==(const user &other) const;
    std::strong_ordering operator<=>(const user &other) const;

private:
    std::string name;
    std::string email;
    std::string id;
    password pw;

    // Serialization
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & name;
    }
};
}

#endif