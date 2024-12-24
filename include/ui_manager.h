/**
 *
 * ui_manager.h
 *
 * UI Manager for p2d
 * Currently not supporting ncurses
 *
 * Author: Sunwoo Na
 *
 */

#ifndef _UI_MANAGER_H_
#define _UI_MANAGER_H_

// #define DONT_USE_NCURSES // Currently not implemented!!!

#ifndef DONT_USE_NCURSES
#include <ncurses.h>
#endif

#include <cstdlib>
#include <string>
#include <vector>

#include "todo_list.h"
#include "user_list.h"

namespace p2d {
class ui_manager {
public:
    ui_manager() = default;
    virtual ~ui_manager() = default;

    int list_selected_index() const;
    int memo_selected_index() const;

    virtual std::pair<std::string, int> show_all_lists(const std::vector<todo_list>& todoLists);
    virtual std::pair<std::string, int> list_memos(const todo_list& todoList);

    virtual void create_list(std::vector<todo_list>& todoLists);
    virtual void create_memo(todo_list& todoList);
    virtual void interact_memo(todo& memo);

    virtual void login(std::unique_ptr<user>& current_user, user_list& all_users);

    virtual void clear();

protected:
    int list_selected = 0; // current selected list
    int memo_selected = 0; // current selected memo
};

#ifndef DONT_USE_NCURSES
class ui_manager_ncurses : public ui_manager {
public:
    ui_manager_ncurses();
    ~ui_manager_ncurses() override;

    virtual std::pair<std::string, int> show_all_lists(const std::vector<todo_list>& todoLists) override;
    virtual std::pair<std::string, int> list_memos(const todo_list& todoList) override;

    virtual void create_list(std::vector<todo_list>& todoLists) override;
    virtual void create_memo(todo_list& todoList) override;
    virtual void interact_memo(todo& memo) override;

    virtual void login(std::unique_ptr<user>& current_user, user_list& all_users) override;

    void clear() override;

private:
    WINDOW* main;
    WINDOW* header;
    WINDOW* list;
    WINDOW* bottom;

    static constexpr int header_size = 3;

    void readline(WINDOW* win, std::string &str) {
        curs_set(1);
        for (int ch; (ch = getch()) != '\n';) {
            if (ch == KEY_BACKSPACE || ch == 127) {
                if (!str.empty()) {
                    str.pop_back();
                    wprintw(win, "\b \b");
                    wrefresh(win);
                }
            } else {
                str.push_back(ch);
                waddch(win, ch);
                wrefresh(win);
            }
        }
        curs_set(0);
    }

private:
    int list_offset = 0; // current offset of the list
    int memo_offset = 0; // current offset of the memo
};
#endif // DONT_USE_NCURSES
}

#endif // _UI_MANAGER_H_