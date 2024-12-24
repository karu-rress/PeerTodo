/**
 *
 * todo_list.cpp
 *
 * List of todo entries
 *
 * Author: Sunwoo Na
 *
 */

#include <sstream>

#include "../include/todo_list.h"

using namespace std;

namespace p2d {
todo_list::todo_list(string_view title)
    : title { title } { }

[[nodiscard]] string &todo_list::get_title() {
    return title;
}

[[nodiscard]] vector<todo> &todo_list::get_todos() {
    return todos;
}

[[nodiscard]] const string &todo_list::get_title() const {
    return title;
}

[[nodiscard]] const vector<todo> &todo_list::get_todos() const {
    return todos;
}

// here id is id of todo
[[nodiscard]] vector<todo>::iterator todo_list::find(int id) {
    return rng::find_if(todos, [id](todo &t) {
        return t.id == id;
    });
}

[[nodiscard]] vector<todo>::const_iterator todo_list::find(int id) const {
    return rng::find_if(todos, [id](const todo &t) {
        return t.id == id;
    });
}

// here id is index of todo
bool todo_list::remove(int id) {
    todos.erase(begin(todos) + id);
    return true;
}

void todo_list::sort(compare_by cmp) {
    rng::sort(todos, cmp);
}

bool todo_list::mark_as_completed(int id) {
    todo &todo = todos[id];
    todo.completed = true;
    return true;
}

bool todo_list::mark_as_incomplete(int id) {
    todo &todo = todos[id];
    todo.completed = false;
    return true;
}

bool todo_list::clear() {
    if (todos.empty()) {
        return false;
    }

    todos.clear();
    return true;
}
}