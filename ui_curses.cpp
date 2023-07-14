#include "ui_curses.hpp"
#include "sig_flags.hpp"

#include <curses.h>

Curses::Curses()
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

    timer = clock::duration::zero();
}

void Curses::say_hello()
{
    erase();
    attron(COLOR_PAIR(pair));
    printw("Hello world!!");
    refresh();
    attroff(COLOR_PAIR(pair));
}

void Curses::main_loop()
{
    say_hello();
    while (!needs_shutdown) {
        auto start = clock::now();

        auto key = getch();
        if (key != ERR && key != 0) {
            needs_shutdown = 1;
        }

        using namespace std::chrono_literals;
        if (timer > 1s) {
            if (pair == 1) {
                pair = 2;
            } else {
                pair = 1;
            }
            say_hello();

            timer = clock::duration::zero();
        }

        timer += clock::now() - start;
    }
}

Curses::~Curses() noexcept
{
    endwin();
}

