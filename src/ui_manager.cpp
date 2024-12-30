/**
 *
 * ui_manager.cpp
 *
 * UI Manager for p2d
 * Currently not supporting ncurses
 *
 * Author: Sunwoo Na
 *
 */

#include <chrono>
#include <format>
#include <fstream>
#include <iostream>

#include <sys/wait.h>
#include <unistd.h>

#include "../include/session.h"
#include "../include/ui_manager.h"

using namespace std;

namespace p2d {
int ui_manager::list_selected_index() const
{
    return list_selected;
}

int ui_manager::memo_selected_index() const
{
    return memo_selected;
}

pair<string, int> ui_manager::show_all_lists(const std::vector<todo_list>& todoLists)
{

    clear();

    cout << format("{} lists\n", todoLists.size());
    cout << "====================\n";

    int i = 1;

    for (const auto& list : todoLists) {
        cout << format("{}. {}", i++, list.get_title()) << '\n';
    }

    cout << "====================\n";
    cout << "Type command (add/remove/select/exit): ";

    std::string command;
    cin >> command;
    cin.get();
    if (command == "add") {
        return { command, -1 };
    } else if (command == "exit") {
        return { command, 0 };
    }

    cin >> list_selected;
    cin.get();

    list_selected--;
    if (command == "remove") {
        return { "remove", list_selected + 1 };
    }
    return { "select", list_selected + 1 };
}

pair<string, int> ui_manager::list_memos(const todo_list& todoList)
{
    clear();

    cout << format("<{}>\n", todoList.get_title());
    cout << format("{} memos\n", todoList.get_todos().size());

    int i = 1;
    for (const auto& memo : todoList.get_todos()) {
        cout << format("{}: {} {} (Deadline: {:})",
            i++,
            (memo.is_completed() ? "[X]" : "[ ]"),
            memo.get_title(),
            memo.get_deadline())
             << '\n';
    }

    cout << "====================\n";
    cout << "Type command (add/remove/edit/exit/check/uncheck): ";

    std::string command;
    cin >> command;
    cin.get();

    if (command == "add") {
        return { command, -1 };
    } else if (command == "exit") {
        return { command, 0 };
    }

    cin >> memo_selected;
    cin.get();
    memo_selected--;
    if (command == "remove") {
        return { "remove", memo_selected + 1 };
    } else if (command == "edit") {
        return { "edit", memo_selected + 1 };
    } else if (command == "check") {
        return { "check", memo_selected + 1 };
    }
    return { "uncheck", memo_selected + 1 };
}

void ui_manager::create_list(std::vector<todo_list>& todoLists)
{
    clear();

    cout << "Enter the title of the list: ";
    string title;
    getline(cin, title);

    todoLists.push_back(todo_list { title });
}

void ui_manager::create_memo(todo_list& todoList)
{
    clear();

    cout << "Enter the title of the memo: ";
    string title;
    getline(cin, title);

    cout << "Enter the deadline (YYYY-MM-DD HH:MM:SS): ";
    string dl;
    getline(cin, dl);

    std::tm tm = {};
    std::istringstream ss(dl);

    // Parse the input string into a std::tm structure
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    // Convert std::tm to std::time_t
    time_t time = std::mktime(&tm);

    // Convert std::time_t to std::chrono::time_point
    auto deadline = chrono::system_clock::from_time_t(time);

    int id = todoList.add(title, "", deadline);

    interact_memo(todoList.get_todos()[id]);
}

void ui_manager::interact_memo(todo& memo)
{
    clear();
    const bool has_nano = system("nano --version > /dev/null 2>&1") == 0;

    // 1. save memo to file
    string memo_file = memo.get_title();
    ofstream fout { memo_file };
    fout << memo.get_description();
    fout.close();

    // 2. edit memo with fork, exec with text editor
    pid_t pid = fork();
    if (pid == 0) { // pid == 0 => child process
        // Run nano with the memo file
        if (has_nano)
            execlp("nano", "nano", memo_file.c_str(), nullptr);
        else // use vi instead
            execlp("vi", "vi", memo_file.c_str(), nullptr);

        cerr << "Error: execlp() failed.\n";
        return;
    } else if (pid > 0) { // pid > 0 => parent process
        wait(nullptr); // wait for the child process to finish
    } else { // pid < 0 => fork failed
        cerr << "Error: fork() failed.\n";
        return;
    }

    // 3. read memo from file
    ifstream fin { memo_file };
    string description;
    getline(fin, description, '\0');
    fin.close();

    // 4. delete temp memo file
    filesystem::remove(memo_file);

    // 5. update memo
    memo.set_description(description);
}

void ui_manager::login(std::unique_ptr<user>& current_user, user_list& all_users)
{
    clear();
    cout << format("Welcome to {}!\n", session::app_name);
    cout << "No login information found. Please login.\n";
    cout << "====================\n";

    string id;
    while (true) {
        cout << "ID: ";
        cin >> id;
        cin.get();

        if (!all_users.contains(id))
            break;

        cout << "ID already exists. Please try again.\n";
    }

    char* pass = getpass("Password: ");
    if (!pass) {
        cout << "Password input error.\n";
        return;
    }

    cout << "Your name: ";
    string name;
    getline(cin, name);

    cout << "Your email: ";
    string email;
    cin >> email;

    all_users.add(user { name, email, id, password { pass } });
    current_user = make_unique<user>(all_users[id]);
}

void ui_manager::clear()
{
    system("clear");
}

//////

#ifndef DONT_USE_NCURSES
ui_manager_ncurses::ui_manager_ncurses()
    : main { initscr() }
{
    cbreak();
    noecho();
    keypad(main, TRUE);
    start_color();

    if (has_colors() == FALSE) {
        endwin();
        printw("Your terminal does not support color\n");
        exit(1);
    }

    // Initialize color pairs
    init_pair(1, COLOR_WHITE, COLOR_BLACK); // unselected
    init_pair(2, COLOR_BLACK, COLOR_WHITE); // selected
    init_pair(3, COLOR_YELLOW, COLOR_BLACK); // header

    // Set window size
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    header = newwin(header_size, max_x, 0, 0);
    list = newwin(max_y - header_size - 1, max_x, header_size, 0);
    bottom = newwin(1, max_x, max_y - 1, 0);

    refresh();
    wrefresh(main);

    keypad(list, TRUE); // Convert arrow keys to special keys
    set_escdelay(30); // Set ESC delay to 30ms

    curs_set(0); // Hide cursor

    wattron(header, COLOR_PAIR(3));
}

ui_manager_ncurses::~ui_manager_ncurses()
{
    delwin(header);
    delwin(list);
    delwin(bottom);
    endwin();
}

std::pair<std::string, int>
ui_manager_ncurses::show_all_lists(const std::vector<todo_list>& todoLists)
{
    wclear(header);
    wclear(list);
    wclear(bottom);

    // Header: Todo List 개수, 유저 이름
    int max_x = getmaxx(header);
    std::string_view title1 = "Todo Lists";
    std::string_view title2 = format("{} lists", todoLists.size());
    int start_x = (max_x - title1.length()) / 2;

    // 빈 칸은 모두 ' '으로 채워야 함
    mvwprintw(header, 0, start_x, "%s", title1.data());
    start_x = (max_x - title2.length()) / 2;
    mvwprintw(header, 1, start_x, "%s", title2.data());
    wrefresh(header);

    // Bottom: 사용법
    max_x = getmaxx(bottom);
    std::string_view usage = "Esc: Exit    Enter: Select    a: Add    Del: Remove";
    start_x = (max_x - usage.length()) / 2;
    mvwprintw(bottom, 0, start_x, "%s", usage.data());
    wrefresh(bottom);

    list_offset = 0; // 첫 번째 리스트부터 출력

    do {
        wclear(list);
        int y = 1;
        auto start = begin(todoLists) + list_offset;
        auto end = start + std::min<size_t>(todoLists.size(), getmaxy(list) - 1);
        for (auto& l : ranges::subrange(start, end)) {
            if (y - 1 == list_selected) {
                wattron(list, COLOR_PAIR(2));
            }

            // TODO:... x max까지 채워야 함.
            std::string title = format("{}: {}", list_offset + y, l.get_title());
            mvwprintw(list, y, 0, "%-*s", getmaxx(list), title.data()); // 제목 출력 및 나머지 공간을 공백으로 채움

            if (y - 1 == list_selected) {
                wattroff(list, COLOR_PAIR(2));
            }

            y++;
        }

        wrefresh(list);

        switch (int ch = wgetch(list); ch) {
        case 'a': // 'a' pressed, add
            return { "add", -1 };

        case KEY_DC: // Del pressed, remove
        case 127:
            return { "remove", list_selected + 1 };

        case 10: // Enter pressed, select
            return { "select", list_selected + 1 };

        case KEY_UP:
            list_selected = std::max(0, list_selected - 1);
            if (list_selected < list_offset) {
                list_offset = std::max(0, list_offset - 1);
            }
            break;

        case KEY_DOWN:
            list_selected = std::min<int>(todoLists.size() - 1, list_selected + 1);
            if (list_selected >= list_offset + getmaxy(list) - 1) {
                list_offset = std::min<int>(todoLists.size() - getmaxy(list) + 1, list_offset + 1);
            }
            break;

        [[unlikely]] case 27: // ESC
            return { "exit", 0 };

        default:
            break;
        }
    } while (true);
}

std::pair<std::string, int>
ui_manager_ncurses::list_memos(const todo_list& todoList)
{
    wclear(header);
    wclear(list);
    wclear(bottom);

    // Header: Todo List 개수, 유저 이름
    int max_x = getmaxx(header);
    std::string title1 = format("Viewing list: {}\n", todoList.get_title());
    std::string_view title2 = format("{} todos\n", todoList.get_todos().size());
    int start_x = (max_x - title1.length()) / 2;
    mvwprintw(header, 0, start_x, "%s", title1.data());
    start_x = (max_x - title2.length()) / 2;
    mvwprintw(header, 1, start_x, "%s", title2.data());
    wrefresh(header);

    // Bottom: 사용법
    max_x = getmaxx(bottom);
    std::string_view usage = "Esc: Exit    Enter: Select    a: Add    e: Edit    Del: Remove    Space: Check/Uncheck";
    start_x = (max_x - usage.length()) / 2;
    mvwprintw(bottom, 0, start_x, "%s", usage.data());
    wrefresh(bottom);

    memo_offset = 0; // 첫 번째 리스트부터 출력

    auto& todos = todoList.get_todos();
    do {
        wclear(list);
        int y = 1;
        auto start = begin(todos) + memo_offset;
        auto end = start + std::min<size_t>(todos.size(), getmaxy(list) - 1);
        for (auto& l : ranges::subrange(start, end)) {
            if (y - 1 == memo_selected) {
                wattron(list, COLOR_PAIR(2));
            }

            std::string title = format("[{}] {}: {}",
                (l.is_completed() ? "X" : " "), memo_offset + y, l.get_title());
            mvwprintw(list, y, 0, "%-*s", getmaxx(list), title.data()); // 제목 출력 및 나머지 공간을 공백으로 채움

            if (y - 1 == memo_selected) {
                wattroff(list, COLOR_PAIR(2));
            }

            y++;
        }

        wrefresh(list);

        switch (int ch = wgetch(list); ch) {
        case 'a': // 'a' pressed, add
            return { "add", -1 };

        case KEY_DC: // Del pressed, remove
        case 127:
            return { "remove", memo_selected + 1 };

        case 10: // Enter pressed, select
            return { "select", memo_selected + 1 };

        case KEY_UP:
            memo_selected = std::max(0, memo_selected - 1);
            if (memo_selected < memo_offset) {
                memo_offset = std::max(0, memo_offset - 1);
            }
            break;

        case KEY_DOWN:
            memo_selected = std::min<int>(todos.size() - 1, memo_selected + 1);
            if (memo_selected >= memo_offset + getmaxy(list) - 1) {
                memo_offset = std::min<int>(todos.size() - getmaxy(list) + 1, memo_offset + 1);
            }
            break;

        // Space
        case ' ':
            if (todos[memo_selected].is_completed()) {
                return { "uncheck", memo_selected + 1 };
            } else {
                return { "check", memo_selected + 1 };
            }

        case 'e': // 'e' pressed, edit
            return { "edit", memo_selected + 1 };

        [[unlikely]] case 27: // ESC
            return { "exit", 0 };

        default:
            break;
        }
    } while (true);
}

void ui_manager_ncurses::create_list(std::vector<todo_list>& todoLists)
{
    clear();

    wprintw(header, "Create a new list\n");
    wprintw(header, "Enter the title of the list");
    wrefresh(header);

    wprintw(list, "Title: ");
    wrefresh(list);

    std::string title;
    readline(list, title);

    todoLists.push_back(todo_list { title });
}

void ui_manager_ncurses::create_memo(todo_list& todoList)
{
    clear();

    wprintw(header, "Create a new memo\n");
    wrefresh(header);

    wprintw(list, "\nTitle: ");
    wrefresh(list);

    std::string title;
    readline(list, title);

    wprintw(list, "\nDeadline (YYYY-MM-DD HH:MM:SS): ");
    wrefresh(list);

    std::string dl;
    readline(list, dl);

    std::tm tm = {};
    std::istringstream ss(dl);

    // Parse the input string into a std::tm structure
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    // Convert std::tm to std::time_t
    time_t time = std::mktime(&tm);

    // Convert std::time_t to std::chrono::time_point
    auto deadline = chrono::system_clock::from_time_t(time);

    int id = todoList.add(title, "", deadline);

    interact_memo(todoList.get_todos()[id]);
}

void ui_manager_ncurses::interact_memo(todo& memo)
{
    clear();
    const bool has_nano = system("nano --version > /dev/null 2>&1") == 0;

    // 1. save memo to file
    string memo_file = memo.get_title();
    ofstream fout { memo_file };
    fout << memo.get_description();
    fout.close();

    def_prog_mode();
    endwin();

    // 2. edit memo with fork, exec with text editor
    pid_t pid = fork();
    if (pid == 0) { // pid == 0 => child process
        // Run nano with the memo file
        if (has_nano)
            execlp("nano", "nano", memo_file.c_str(), nullptr);
        else // use vi instead
            execlp("vi", "vi", memo_file.c_str(), nullptr);

        cerr << "Error: execlp() failed.\n";
        _exit(1);
    } else if (pid > 0) { // pid > 0 => parent process
        wait(nullptr); // wait for the child process to finish
        reset_prog_mode();
        refresh();
        curs_set(0);
    } else { // pid < 0 => fork failed
        cerr << "Error: fork() failed.\n";
        return;
    }

    // 3. read memo from file
    ifstream fin { memo_file };
    string description;
    getline(fin, description, '\0');
    fin.close();

    // 4. delete temp memo file
    filesystem::remove(memo_file);

    // 5. update memo
    memo.set_description(description);
}

void ui_manager_ncurses::login(std::unique_ptr<user>& current_user, user_list& all_users)
{
    ui_manager::login(current_user, all_users);
}

void ui_manager_ncurses::clear()
{
    wclear(main);
    wclear(header);
    wclear(list);
    wclear(bottom);
    wrefresh(main);
    wrefresh(header);
    wrefresh(list);
    wrefresh(bottom);
}
#endif

}