/**
 *
 * todo_list.h
 *
 * List of todo entries
 *
 * Author: Sunwoo Na
 *
 */

#ifndef _TODO_LIST_H_
#define _TODO_LIST_H_

#include <algorithm>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

#include "todo.h"

namespace rng = std::ranges;

namespace p2d {
class todo_list {
    using compare_by = std::function<bool(const todo &, const todo &)>;
    friend class boost::serialization::access;

public:
    // Constructors
    todo_list(std::string_view title);
    todo_list(todo_list &&rhs) noexcept = default;

    // Disable copy semantics
    todo_list(const todo_list &rhs) = delete;
    todo_list &operator=(const todo_list &rhs) = delete;

    // Operators
    todo_list &operator=(todo_list &&rhs) noexcept = default;

    // Getters
    [[nodiscard]] std::string &get_title();
    [[nodiscard]] std::vector<todo> &get_todos();
    [[nodiscard]] const std::string &get_title() const;
    [[nodiscard]] const std::vector<todo> &get_todos() const;

    [[nodiscard]] std::vector<todo>::iterator find(int id);
    [[nodiscard]] std::vector<todo>::const_iterator find(int id) const;

    // Member functions
    template <typename... Args>
    int add(Args&& ...args) {
        todo new_todo { current_id++, std::forward<Args>(args)... };
        int id = new_todo.id;
        todos.push_back(std::move(new_todo));
        sort();

        // return index, find it using lambda
        return [&] {
            auto it = rng::find_if(todos, [id](const todo &t) {
                return t.id == id;
            });
            return std::distance(begin(todos), it);
        }();
    }

    bool remove(int id);

    // Comparison options:
    //   order::by_deadline
    //   order::by_created
    //   order::by_completed
    //   order::by_title
    void sort(compare_by cmp = order::by_deadline {});

    bool mark_as_completed(int id);
    bool mark_as_incomplete(int id);

    bool clear();

private:
    std::string title;
    std::vector<todo> todos;

    static constexpr std::string_view box_unchecked = "☐";
    static constexpr std::string_view box_checked = "☑";

    todo_list() = default; // for serialization

    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & title;
        ar & todos;
    }

    int current_id = 0;
};
}

#endif