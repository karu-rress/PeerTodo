/**
 *
 * session.h
 *
 * Manages session for p2d
 *
 * Author: Sunwoo Na
 *
 */

#ifndef _SESSION_H_
#define _SESSION_H_

#include <filesystem>
#include <memory>
#include <string_view>
#include <type_traits>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "todo_list.h"
#include "ui_manager.h"
#include "user_list.h"

namespace p2d {
template <typename T>
concept SerializableData = std::is_same_v<T, user> || std::is_same_v<T, user_list> || std::is_same_v<T, std::vector<todo_list>>;

class session {
public:
    session(ui_manager &ui);
    ~session();

    void load_login();
    void load_user();
    void load_todo();

    void save_login();
    void save_user();
    void save_todo();

    void run();

    [[nodiscard]] std::string serialize_login() const;
    [[nodiscard]] std::string serialize_user() const;
    [[nodiscard]] std::string serialize_todo() const;

    void parse_binary_login(const std::string &data);
    void parse_binary_user(const std::string &data);
    void parse_binary_todo(const std::string &data);

    [[maybe_unused]] static constexpr std::string_view app_name = "PeerTodo";

private:
    ui_manager &ui;

    std::unique_ptr<user> current_user;
    user_list all_users;

    std::vector<todo_list> todo_lists;
    std::filesystem::path data_path;

    static constexpr std::string_view login_file = "login.bin";
    static constexpr std::string_view user_file = "user.bin";
    static constexpr std::string_view todo_file = "todo.bin";

    [[nodiscard]] std::streamsize file_size(std::ifstream &fin) const;

    template <SerializableData T>
    [[nodiscard]] static std::string serialize(const T &obj) {
        std::ostringstream oss;
        boost::archive::binary_oarchive oa { oss };
        oa << obj;
        return oss.str();
    }

    template <SerializableData T>
    static void parse_binary(T &obj, const std::string &data) {
        std::istringstream iss { data };
        boost::archive::binary_iarchive ia { iss };
        ia >> obj;
    }
};
}

#endif