#include "sdl_util.hpp"

#include <curses.h>
#include <SDL2/SDL.h>

#include <clocale>
#include <csignal>
#include <sstream>
#include <stdexcept>
#include <chrono>

volatile sig_atomic_t interrupted = 0;
volatile sig_atomic_t needs_shutdown = 0;
volatile sig_atomic_t needs_resize = 0;

void sig_int(int sig)
{
    interrupted = 1;
}

void sig_shutdown(int sig)
{
    needs_shutdown = 1;
}

void sig_winch(int sig)
{
    needs_resize = 1;
}

int main(int argc, char* argv[])
{
    int err = 0;

    setlocale(LC_CTYPE, "");
    setlocale(LC_COLLATE, "");

    char vdriver[] = "SDL_VIDEODRIVER=dummy";
    putenv(vdriver);

    sdl_check(SDL_Init(SDL_INIT_AUDIO), "failed to initialize");

    struct sigaction act;

    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = sig_int;
    sigaction(SIGINT, &act, nullptr);

    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = sig_shutdown;
    sigaction(SIGHUP, &act, nullptr);
    sigaction(SIGTERM, &act, nullptr);

    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &act, nullptr);

    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = sig_winch;
    sigaction(SIGWINCH, &act, nullptr);

    initscr();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    int pair = 1;

    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    halfdelay(5);
    noecho();
    curs_set(0);

    using namespace std::chrono_literals;
    using clock = std::chrono::steady_clock;
    clock::duration timer = clock::duration::zero();
    attron(COLOR_PAIR(pair));
    printw("Hello world!!");
    refresh();
    attroff(COLOR_PAIR(pair));
    while (!needs_shutdown) {
        auto start = clock::now();

        auto key = getch();
        if (key != ERR && key != 0) {
            needs_shutdown = 1;
        }

        if (timer > 1s) {
            if (pair == 1) {
                pair = 2;
            } else {
                pair = 1;
            }
            erase();
            attron(COLOR_PAIR(pair));
            printw("Hello world!!");
            refresh();
            attroff(COLOR_PAIR(pair));
            timer = clock::duration::zero();
        }

        timer += clock::now() - start;
    }

    endwin();

    SDL_Quit();

    return err;
}
