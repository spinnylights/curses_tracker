#include "sig_flags.hpp"
#include "ui_curses.hpp"
#include "audio_sdl.hpp"

#include <clocale>
#include <csignal>
#include <sstream>
#include <stdexcept>

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

    Audio aud {};

    Curses cur {};
    cur.main_loop();

    return err;
}
