/**
 *
 * todo.h
 *
 * Todo class with ordering structs
 *
 * Author: Sunwoo Na
 *
 */

#ifndef _TODO_H_
#define _TODO_H_

#include <chrono>
#include <compare>
#include <iostream>
#include <string>
#include <string_view>

#include <boost/serialization/access.hpp>
#include <boost/serialization/binary_object.hpp>

#include "user.h"

namespace p2d {
// Forward declaration
class todo;
namespace order {
    struct by_deadline {
        bool operator()(const todo &lhs, const todo &rhs) const;
    };
    struct by_created {
        bool operator()(const todo &lhs, const todo &rhs) const;
    };
    struct by_completed {
        bool operator()(const todo &lhs, const todo &rhs) const;
    };
    struct by_title {
        bool operator()(const todo &lhs, const todo &rhs) const;
    };
}

class todo {
    using time_pt = std::chrono::time_point<std::chrono::system_clock>;
    friend class todo_list;
    friend class boost::serialization::access;

public:
    // Constructors
    todo(int id,
        std::string_view title,
        std::string_view description,
        const time_pt &deadline);
    todo(int id,
        std::string_view title,
        std::string_view description,
        const time_pt &created,
        const time_pt &deadline,
        const bool completed = false);
    todo(todo &&rhs) noexcept = default;

    // Disable copy semantics
    todo(const todo &rhs) = delete;
    todo &operator=(const todo &rhs) = delete;

    // Operators
    todo &operator=(todo &&rhs) noexcept = default;
    // it will be compared with namespace order
    // auto operator<=>(const todo &rhs) const;

    // Getters
    [[nodiscard]] int get_id() const;
    [[nodiscard]] const std::string &get_title() const;
    [[nodiscard]] const std::string &get_description() const;
    [[nodiscard]] const time_pt &get_created() const;
    [[nodiscard]] const time_pt &get_deadline() const;
    [[nodiscard]] bool is_completed() const;

    // Setters
    void set_title(std::string_view title);
    void set_description(std::string_view description);

    void set_deadline(const time_pt &deadline);
    void set_completed(bool completed);

private:
    int id;

    time_pt created;
    time_pt deadline;

    std::string title;
    std::string description;

    bool completed = false;

    todo() = default; // Not accessible except for serialization

    // Serialization
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & id;
        ar & boost::serialization::make_binary_object(&created, sizeof(created));;
        ar & boost::serialization::make_binary_object(&deadline, sizeof(deadline));
        ar & title;
        ar & description;
        ar & completed;
    }
};
}

#endif