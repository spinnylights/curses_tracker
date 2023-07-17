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

void set_handlers()
{
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
}

int main(int argc, char* argv[])
{
    setlocale(LC_CTYPE, "");
    setlocale(LC_COLLATE, "");

    set_handlers();

    Audio aud;

    Curses cur;

    using clock = std::chrono::steady_clock;
    clock::duration timer = clock::duration::zero();

    cur.say_hello(aud);
    while (!needs_shutdown) {
        auto start = clock::now();

        cur.say_hello(aud);

        cur.getkey();

        //using namespace std::chrono_literals;
        //if (timer > 1s) {
        //    cur.swap_pair();
        //    cur.say_hello();

        //    timer = clock::duration::zero();
        //}

        //timer += clock::now() - start;
    }

    return 0;
}
