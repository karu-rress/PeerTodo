/**
 *
 * session.cpp
 *
 * Manages session for p2d
 *
 * Author: Sunwoo Na
 *
 */

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>

#include "../include/session.h"

using namespace std;
using namespace boost::archive;
namespace fs = std::filesystem;

namespace p2d {
session::session(ui_manager &ui)
    : ui { ui } {
    data_path = fs::path { getenv("HOME") } / ".local" / "share" / "p2d";
    if (!fs::exists(data_path)) {
        fs::create_directories(data_path);
        return;
    }

    load_login();
    load_user();
    load_todo();
}

session::~session() {
    save_login();
    save_user();
    save_todo();
}

void session::load_login() {
    if (fs::exists(data_path / login_file)) {
        ifstream fin { data_path / login_file, ios::binary };
        auto size = file_size(fin);

        string buf(size, '\0');
        fin.read(&buf[0], size);
        parse_binary_login(buf);
    }
}

void session::load_user() {
    if (fs::exists(data_path / user_file)) {
        ifstream fin { data_path / user_file, ios::binary };
        auto size = file_size(fin);

        string buf(size, '\0');
        fin.read(&buf[0], size);
        parse_binary_user(buf);
    }
}

void session::load_todo() {
    if (fs::exists(data_path / todo_file)) {
        ifstream fin { data_path / todo_file, ios::binary };
        auto size = file_size(fin);

        string buf(size, '\0');
        fin.read(&buf[0], size);
        parse_binary_todo(buf);
    }
}

void session::save_login() {
    if (current_user) {
        ofstream fout { data_path / login_file, ios::binary };
        fout << serialize_login();
    }
}

void session::save_user() {
    ofstream fout { data_path / user_file, ios::binary };
    fout << serialize_user();
}

void session::save_todo() {
    ofstream fout { data_path / todo_file, ios::binary };
    fout << serialize_todo();
}

void session::run() {
    if (!current_user) {
        ui.login(current_user, all_users);
    }

    while (true) {
        // Main page: show all lists
        if (auto ret = ui.show_all_lists(todo_lists); ret.second == 0)
            break; // if quit
        else if (ret.second < 0) {
            ui.create_list(todo_lists);
            continue;
        }
        else if (ret.first == "remove") {
            todo_lists.erase(begin(todo_lists) + ui.list_selected_index());
            continue;
        }

        while (true) {
            // Show the selected list
            auto list = begin(todo_lists) + ui.list_selected_index();
            if (auto ret = ui.list_memos(*list); ret.second == 0)
                break; // if back
            else if (ret.second < 0) {
                ui.create_memo(*list);
                continue;
            }
            else if (ret.first == "remove") {
                list->remove(ui.memo_selected_index());
                continue;
            }
            else if (ret.first == "check") {
                list->mark_as_completed(ui.memo_selected_index());
                continue;
            }
            else if (ret.first == "uncheck") {
                list->mark_as_incomplete(ui.memo_selected_index());
                continue;
            }

            // Show the selected memo
            auto memo = begin(list->get_todos()) + ui.memo_selected_index();
            ui.interact_memo(*memo);
        }
    }
}

[[nodiscard]] std::string session::serialize_login() const {
    return serialize(*current_user);
}

[[nodiscard]] std::string session::serialize_user() const {
    return serialize(all_users);
}

[[nodiscard]] std::string session::serialize_todo() const {
    return serialize(todo_lists);
}

void session::parse_binary_login(const std::string &data) {
    user u;
    parse_binary(u, data);
    current_user = make_unique<user>(move(u));
}

void session::parse_binary_user(const std::string &data) {
    parse_binary(all_users, data);
}

void session::parse_binary_todo(const std::string &data) {
    parse_binary(todo_lists, data);
}

[[nodiscard]] std::streamsize session::file_size(std::ifstream &fin) const {
    fin.seekg(0, ios::end);
    auto size = fin.tellg();
    fin.seekg(0, ios::beg);

    return size;
}
}