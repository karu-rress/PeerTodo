/**
 *
 * main.cpp
 *
 * main function for the p2d
 *
 * Author: Sunwoo Na
 *
 */

#include "include/session.h"
#include "include/ui_manager.h"

// Verify the OS
#ifdef __linux__
constexpr std::string_view OS = "Linux";
#elif _WIN32
constexpr std::string_view OS = "Windows";
#elif __APPLE__
constexpr std::string_view OS = "macOS";
#endif

static_assert(OS == "Linux" || OS == "macOS",
    "This program is only supported on Linux and macOS.");

using namespace p2d;
using namespace std;

int main(int argc, char *argv[]) {
    ui_manager_ncurses ui;
    session sess { ui };

    sess.run();

    return 0;
}