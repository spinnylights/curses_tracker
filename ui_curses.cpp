#include "ui_curses.hpp"
#include "sig_flags.hpp"
#include "chk_errno.hpp"
#include "event.hpp"

#include <termios.h>

Curses::Curses()
{
    initscr();

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_color(20, 1000, 350, 150);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);

    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    halfdelay(5);
    noecho();
    curs_set(0);

    update_winsz();
}

const std::string scoreline1 = "8.25 AORD";
const std::string scoreline2 = "7.05 A";

void Curses::say_hello(const Audio& aud)
{
    werase(win);

    wattron(win, COLOR_PAIR(1));
    box(win, 0, 0);
    wprintw(win, "%dx%d/%c/%d/%0.5f",
            w(), h(), key, aud.sr(), Note::edo_pitch(8, 40, 0.5));

    unsigned lrwidth = scoreline1.length() + 4;
    unsigned ledge = 1;
    unsigned redge = lrwidth + ledge;
    unsigned times = w() / (lrwidth + 1) + 1;

    unsigned cnt = 0;
    for (unsigned i = 0; i < times; ++i) {
        std::string s = scoreline2;

        if (i % 2 == 0) {
            s = scoreline1;
        }

        if (i == times - 1) {
            s = scoreline2.substr(0, w() - ledge - 5);
        }

        for (unsigned j = 1; j < h() - 1; ++j) {
            wmove(win, j, ledge);
            if (cnt % 4 == 0) {
                if (cnt == 0) {
                    wattron(win, COLOR_PAIR(2));
                } else {
                    wattron(win, COLOR_PAIR(3));
                }
            }
            wprintw(win, "%03X", cnt);
            if (cnt % 3 == 0) {
                wprintw(win, " %s", s.c_str());
            }
            if (cnt % 4 == 0) {
                wattron(win, COLOR_PAIR(1));
            }
            ++cnt;
        }

        if (i != times - 1) {
            wmove(win, 0, redge);
            waddch(win, ACS_TTEE);
            wmove(win, 1, redge);
            wvline(win, ACS_VLINE, h() - 2);
            wmove(win, h() -1, redge);
            waddch(win, ACS_BTEE);
        }

        ledge = redge + 1;
        redge = lrwidth + ledge;
    }

    wrefresh(win);
}

void Curses::getkey()
{
    key = wgetch(win);

    if (key == ERR || key == 0) {
        key = '!';
    } else if (key == KEY_RESIZE) {
        update_winsz();
    } else if (key == 'q') {
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

void Curses::update_winsz()
{
    chk_errno(ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1, "Curses::update_win");
    if (win) {
        delwin(win);
    }
    win = newwin(h(), w(), 0, 0);
}

Curses::~Curses() noexcept
{
    delwin(win);
    endwin();
}
