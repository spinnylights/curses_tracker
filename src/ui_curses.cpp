#include "ui_curses.hpp"
#include "sig_flags.hpp"
#include "chk_errno.hpp"
#include "note.hpp"

#include <termios.h>

Curses::Curses()
{
    initscr();

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_color(20, 1000, 350, 150);
    short grey = 200;
    init_color(30, grey, grey, grey);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_MAGENTA, 30);

    init_color(50, 1000, 1000, 100);
    init_color(51, 1000, 1000, 380);
    init_color(52, 1000, 1000, 460);
    init_color(53, 1000, 1000, 550);
    init_color(54, 1000, 1000, 630);
    init_color(55, 1000, 1000, 750);
    init_color(56, 1000, 1000, 800);
    init_color(57, 1000, 1000, 900);
    init_pair(80, 50, COLOR_BLACK);
    init_pair(81, 51, COLOR_BLACK);
    init_pair(82, 52, COLOR_BLACK);
    init_pair(83, 53, COLOR_BLACK);
    init_pair(84, 54, COLOR_BLACK);
    init_pair(85, 55, COLOR_BLACK);
    init_pair(86, 56, COLOR_BLACK);
    init_pair(87, 57, COLOR_BLACK);

    init_color(31, 600, 250, 50);
    init_color(32, 575, 225, 25);

    init_pair(20, COLOR_YELLOW, COLOR_BLUE);
    init_pair(21, COLOR_CYAN, 31);
    init_pair(22, COLOR_CYAN, 32);

    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    halfdelay(1);
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
                    wattron(win, COLOR_PAIR(flash_pair));
                } else {
                    wattron(win, COLOR_PAIR(3));
                }
            }
            wprintw(win, "%03X", cnt);
            if (cnt % 3 == 0) {
                wprintw(win, " %s", s.c_str());
                waddch(win, ACS_UARROW);
                waddch(win, ACS_DARROW);
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

    wmove(win, 0, 0);
    Note note {8, 40, 0.5};
    std::string frac_s = note.pch();
    wprintw(win, "%dx%d/%c/%d/%0.5f/%s",
            w(), h(), key, aud.sr(), note.freq(), frac_s.c_str());

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
