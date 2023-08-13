#include "ui_curses.hpp"
#include "sig_flags.hpp"
#include "chk_errno.hpp"
#include "note.hpp"

#include <termios.h>

#include <fstream>
#include <sstream>

struct rgbcol {
    short r;
    short g;
    short b;
};

static constexpr std::array<struct rgbcol, 64> rgbcols {
    rgbcol {0, 0, 0},
    rgbcol {0, 0, 334},
    rgbcol {0, 334, 0},
    rgbcol {334, 0, 0},
    rgbcol {0, 0, 667},
    rgbcol {0, 667, 0},
    rgbcol {667, 0, 0},
    rgbcol {0, 334, 334},
    rgbcol {334, 0, 334},
    rgbcol {334, 334, 0},
    rgbcol {0, 0, 1000},
    rgbcol {0, 1000, 0},
    rgbcol {1000, 0, 0},
    rgbcol {0, 334, 667},
    rgbcol {0, 667, 334},
    rgbcol {334, 0, 667},
    rgbcol {334, 667, 0},
    rgbcol {667, 0, 334},
    rgbcol {667, 334, 0},
    rgbcol {334, 334, 334},
    rgbcol {0, 334, 1000},
    rgbcol {0, 667, 667},
    rgbcol {0, 1000, 334},
    rgbcol {334, 0, 1000},
    rgbcol {334, 1000, 0},
    rgbcol {667, 0, 667},
    rgbcol {667, 667, 0},
    rgbcol {1000, 0, 334},
    rgbcol {1000, 334, 0},
    rgbcol {334, 334, 667},
    rgbcol {334, 667, 334},
    rgbcol {667, 334, 334},
    rgbcol {0, 667, 1000},
    rgbcol {0, 1000, 667},
    rgbcol {667, 0, 1000},
    rgbcol {667, 1000, 0},
    rgbcol {1000, 0, 667},
    rgbcol {1000, 667, 0},
    rgbcol {334, 334, 1000},
    rgbcol {334, 667, 667},
    rgbcol {334, 1000, 334},
    rgbcol {667, 334, 667},
    rgbcol {667, 667, 334},
    rgbcol {1000, 334, 334},
    rgbcol {0, 1000, 1000},
    rgbcol {1000, 0, 1000},
    rgbcol {1000, 1000, 0},
    rgbcol {334, 667, 1000},
    rgbcol {334, 1000, 667},
    rgbcol {667, 334, 1000},
    rgbcol {667, 667, 667},
    rgbcol {667, 1000, 334},
    rgbcol {1000, 334, 667},
    rgbcol {1000, 667, 334},
    rgbcol {334, 1000, 1000},
    rgbcol {667, 667, 1000},
    rgbcol {667, 1000, 667},
    rgbcol {1000, 334, 1000},
    rgbcol {1000, 667, 667},
    rgbcol {1000, 1000, 334},
    rgbcol {667, 1000, 1000},
    rgbcol {1000, 667, 1000},
    rgbcol {1000, 1000, 667},
    rgbcol {1000, 1000, 1000},
};

Curses::Curses()
{
    initscr();

    start_color();

    reset_color_pairs();

    for (size_t i = 0; i < rgbcols.size(); ++i) {
        init_color(i, rgbcols[i].r, rgbcols[i].g, rgbcols[i].b);
    }

    static constexpr size_t col_cnt = rgbcols.size();

    for (size_t j = 0; j < 4; ++j) {
        for (size_t i = 0; i < rgbcols.size(); ++i) {
            init_pair(i + 1 + rgbcols.size()*j,
                      ((rgbcols.size() - 1 - i) + (rgbcols.size()*j/4))
                          % rgbcols.size(),
                      i);
        }
    }

    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    halfdelay(7);
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

void Curses::print_color_chart()
{
    static constexpr short text_cpair = 0xe5;
    for (unsigned i = 0; i < rgbcols.size() / 16; ++i) {
        wmove(win, 0, i*16);
        wattron(win, COLOR_PAIR(text_cpair));
        wprintw(win, "%d", i);
    }

    for (unsigned i = 1; i <= rgbcols.size(); ++i) {
        wmove(win, 1, i - 1);
        wattron(win, COLOR_PAIR(text_cpair));
        wprintw(win, "%X", (i - 1) % 16);
    }

    for (size_t j = 0; j < 4; ++j) {
        for (size_t i = 1; i <= rgbcols.size(); ++i) {
            wmove(win, j+2, i - 1);
            wattron(win, COLOR_PAIR(i + rgbcols.size()*j));
            waddch(win, 'X');
        }
        wmove(win, j+2, rgbcols.size());
        wattron(win, COLOR_PAIR(text_cpair));
        wprintw(win, "%02X", static_cast<unsigned>(j)*64);
    }

    wmove(win, 6, 0);
    wattron(win, COLOR_PAIR(text_cpair));
    wprintw(win, "add one (curses reserves pair 0)");
}

Curses::~Curses() noexcept
{
    reset_color_pairs();
    delwin(win);
    endwin();
}
