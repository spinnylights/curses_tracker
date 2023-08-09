#include "sig_flags.hpp"
#include "ui_curses.hpp"
#include "audio_sdl.hpp"
#include "curves.hpp"
#include "exceptions.hpp"
#include "curses_curve_view.hpp"
#include "synth.hpp"
#include "defs.hpp"

#include <clocale>
#include <csignal>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <ratio>
#include <iomanip>

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
    auto hz_840 = tics(431.9489422711106);
    std::cout << hz_840.count() << '\n';
    setlocale(LC_CTYPE, "");
    setlocale(LC_COLLATE, "");

    set_handlers();

    Audio aud {{}};

    Curves cs {":memory:", aud.srf()};
    auto cn = std::make_shared<Curve>(cs.newc());
    auto cn2 = std::make_shared<Curve>(cs.newc());
    std::string curve_desc_low = "soidser 1.1 2.3 0.02";
    std::string curve_desc_high = "soidser 10.75 10 0.52";
    cn->parse(curve_desc_low);
    cn2->parse(curve_desc_high);
    //cn->parse(".5  segs -5 -1 1");
    //cn->parse(".75 soid 1 .625 0.25 0.5");
    //cn->parse("1   segs 5 .75 -1");

    auto syn = std::make_shared<Synth>(cs,
                                       curve_desc_low,
                                       curve_desc_high,
                                       aud.srf());

    aud.add_synth(syn);
    aud.start_playback();

    Curses cur;

    static const std::string prog_name = "curses_tracker";

    try {
        using clock = std::chrono::steady_clock;
        clock::duration timer_anim = clock::duration::zero();
        clock::duration timer_flash = clock::duration::zero();

        bool flipped = false;
        unsigned long sawl = 1;

        int flash_pair = 80;

        cur.say_hello(aud);
        while (!needs_shutdown) {
            auto start = clock::now();

            cur.say_hello(aud);

            static constexpr int ccv_w = 130;
            int mh = 60;
            int mw = ccv_w * 2;

            int ccvn_col  = 20;
            int ccvn2_col = 4;

            if (syn->high_chord()) {
                ccvn_col  = 4;
                ccvn2_col = 21;
            } else {
                ccvn_col  = 20;
                ccvn2_col = 4;
            }

            constexpr int div = 2;
            CursesCurveView ccvn (*cn,
                                  60,
                                  ccv_w*2/div,
                                  (LINES - mh)/2,
                                  (COLS - mw)/2,
                                  ccvn_col);

            CursesCurveView ccvn2 (*cn2,
                                   60,
                                   ccv_w*2/div,
                                   (LINES - mh)/2,
                                   (COLS - mw)/2 + ccv_w*2/div,
                                   ccvn2_col);

            cur.getkey();

            using namespace std::chrono_literals;
            if (timer_anim > 1s) {
                timer_anim = clock::duration::zero();
            }

            if (timer_flash > 200ms) {
                if (++flash_pair > 87) {
                    flash_pair = 80;
                }
                timer_flash = clock::duration::zero();
            }

            cur.flash_pair = flash_pair;

            timer_anim += clock::now() - start;
            timer_flash += clock::now() - start;
        }

        while (!syn->shutdown()) {}
    } catch (runtime_error e) {
        cur.~Curses();
        std::cerr << e.what();
        abort();
    }

    //std::ofstream log;
    //log.open("samps_log.txt", std::ios::app);
    //log << "max amp: " << syn->amp_ceil() << "\n";
    //log.close();

    return 0;
}
