/**
 *
 * todo.cpp
 *
 * Todo class with ordering structs
 *
 * Author: Sunwoo Na
 *
 */

#include <format>

#include "../include/todo.h"

using namespace std;

namespace p2d::order {

bool by_deadline::operator()(const todo &lhs, const todo &rhs) const {
    return lhs.get_deadline() < rhs.get_deadline();
}

bool by_created::operator()(const todo &lhs, const todo &rhs) const {
    return lhs.get_created() < rhs.get_created();
}

bool by_completed::operator()(const todo &lhs, const todo &rhs) const {
    return lhs.is_completed() < rhs.is_completed();
}

bool by_title::operator()(const todo &lhs, const todo &rhs) const {
    return lhs.get_title() < rhs.get_title();
}
}

namespace p2d {
// Constructors
todo::todo(int id, string_view title, string_view description, const time_pt &deadline)
    : id { id }
    , title { title }
    , description { description }
    , deadline { deadline } {
    created = chrono::system_clock::now();
}

todo::todo(int id, string_view title, string_view description, const time_pt &created, const time_pt &deadline, const bool completed)
    : id { id }
    , title { title }
    , description { description }
    , created { created }
    , deadline { deadline }
    , completed { completed } { }

// Getters
[[nodiscard]] int todo::get_id() const {
    return id;
}

[[nodiscard]] const string &todo::get_title() const {
    return title;
}

[[nodiscard]] const string &todo::get_description() const {
    return description;
}

[[nodiscard]] const todo::time_pt &todo::get_created() const {
    return created;
}

[[nodiscard]] const todo::time_pt &todo::get_deadline() const {
    return deadline;
}

[[nodiscard]] bool todo::is_completed() const {
    return completed;
}

// Setters
void todo::set_title(string_view title) {
    this->title = title;
}

void todo::set_description(string_view description) {
    this->description = description;
}

void todo::set_deadline(const time_pt &deadline) {
    this->deadline = deadline;
}

void todo::set_completed(bool completed) {
    this->completed = completed;
}

ostream &operator<<(ostream &os, const todo &td) {
    os << "Title: " << td.get_title() << endl;
    os << "Description: " << td.get_description() << endl;
    os << "Created: " << format("{:%Y-%m-%d}", td.get_created()) << endl;
    os << "Deadline: " << format("{:%Y-%m-%d}", td.get_deadline()) << endl;
    os << "Completed: " << (td.is_completed() ? "Yes" : "No") << endl;

    return os;
}

}