#include "ui_curses.hpp"
#include "sig_flags.hpp"

#include <curses.h>

Curses::Curses()
    : nv {"8", "522301"}
{
    initscr();

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);

    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    halfdelay(5);
    noecho();
    curs_set(0);
}

void Curses::say_hello()
{
    erase();
    attron(COLOR_PAIR(pair));
    printw("%s", nv.str().c_str());
    refresh();
    attroff(COLOR_PAIR(pair));
}

void Curses::getkey()
{
    auto key = getch();
    if (key != ERR && key != 0) {
        needs_shutdown = 1;
    }
}

void Curses::swap_pair()
{
    if (pair == 1) {
        pair = 2;
    } else {
        pair = 1;
    }
}

Curses::~Curses() noexcept
{
    endwin();
}
