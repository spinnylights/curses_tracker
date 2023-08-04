#include "sig_flags.hpp"
#include "ui_curses.hpp"
#include "audio_sdl.hpp"
#include "curves.hpp"
#include "exceptions.hpp"
#include "curses_curve_view.hpp"

#include <clocale>
#include <csignal>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <filesystem>

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

    Curves cs {":memory:"};
    auto cn = std::make_shared<Curve>(cs.newc());
    cn->parse(".5  segs -5 -1 1");
    cn->parse(".75 soid 1 .625 0.25 0.5");
    cn->parse("1   segs 5 .75 -1");

    Audio aud {{cn}};

    Curses cur;

    static const std::string prog_name = "curses_tracker";

    try {
        using clock = std::chrono::steady_clock;
        clock::duration timer_anim = clock::duration::zero();
        clock::duration timer_flash = clock::duration::zero();

        bool flipped = false;
        unsigned long sawl = 1;

        cur.say_hello(aud);
        while (!needs_shutdown) {
            auto start = clock::now();

            cur.say_hello(aud);

            static constexpr int ccv_w = 130;
            int mh = 60;
            int mw = ccv_w * 2;

            constexpr int div = 1;
            CursesCurveView ccvn (*cn,
                                  60/div,
                                  ccv_w*2/div,
                                  (LINES - mh)/2,
                                  (COLS - mw)/2);

            cur.getkey();

            using namespace std::chrono_literals;
            if (timer_anim > 1s) {
                timer_anim = clock::duration::zero();
            }

            if (timer_flash > 200ms) {
                if (++cur.flash_pair > 87) {
                    cur.flash_pair = 80;
                }
                timer_flash = clock::duration::zero();
            }

            timer_anim += clock::now() - start;
            timer_flash += clock::now() - start;
        }
    } catch (runtime_error e) {
        cur.~Curses();
        std::cerr << e.what();
        abort();
    }

    return 0;
}
