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

    Audio aud;

    Curses cur;

    static const std::string prog_name = "curses_tracker";

    //namespace fs = std::filesystem;

    //fs::path home_dir = getenv("HOME");
    //if (home_dir.empty()) {
    //    throw std::runtime_error("HOME env variable not set");
    //}

    //fs::path main_config_dir = home_dir / ".config";
    //if (!fs::is_directory(main_config_dir)) {
    //    throw std::runtime_error("HOME/.config is not a directory");
    //}

    //fs::path config_dir = main_config_dir / prog_name;
    //if (fs::exists(config_dir) && !fs::is_directory(config_dir)) {
    //    throw std::runtime_error(config_dir.string() + " is not a directory");
    //} else if (!fs::exists(config_dir)) {
    //    fs::create_directory(config_dir);
    //}

    try {
        //fs::path curves_db_path = config_dir / "curves.sqlite3";
        //Curves cs {curves_db_path};
        Curves cs {":memory:"};
        auto ct = cs.newc()
//                   .transeg();
//                   .transeg(0.0, -1.0);
                   .transeg(-7.5, -1.0, 1.0);
//                   .sine();
//                   .save();
        //auto curves_db = std::make_shared<CurveDB>(curves_db_path);
        //Curve c {curves_db, "meow"};
        //c.transeg(3.0, -1.0);
        //c.save();
        auto cn = cs.newc()
                    .sine()
                    .save();

        cn.transeg().save();

        auto cn_id = cn.id;
        auto cn2 = cs.get(cn_id);

        using clock = std::chrono::steady_clock;
        clock::duration timer_anim = clock::duration::zero();
        clock::duration timer_flash = clock::duration::zero();

        bool flipped = false;
        unsigned long sawl = 1;
        cn2.sine(sawl).save();

        std::vector<Curve::Segs> csegs = {
             {0, -1},
             {0, -1},
        };

        auto cn3 = cs.newc();
        //             .transeg(0, -1, 1)
        //             .save();
        //segs.process(cn3);
        Curve::CurveAlg& a = csegs[0];
        double start = 0.0;
        double dist = 0.5;
        a.process(cn3, dist, start);
        start += dist;
        dist += dist;
        a = csegs[1];
        a.process(cn3, dist, start);
        cn3.save();

        cur.say_hello(aud);
        while (!needs_shutdown) {
            auto start = clock::now();

            cur.say_hello(aud);

            static constexpr int ccv_w = 130;
            //CursesCurveView ccvt (ct, 40, ccv_w, 10, 10);
            //CursesCurveView ccvn (cn, 40, ccv_w, 29, 25 + ccv_w);
            //CursesCurveView ccvt (ct, 40, ccv_w, 10, 10);
            int mh = 60;
            int mw = ccv_w * 2;
            cn2 = cs.get(cn.id);
            //CursesCurveView ccvn (cn2,
            //                      60,
            //                      ccv_w*2,
            //                      (LINES - mh)/2,
            //                      (COLS - mw)/2);

            constexpr int div = 1;
            CursesCurveView ccvn (cn3,
                                  60/div,
                                  ccv_w*2/div,
                                  (LINES - mh)/2,
                                  (COLS - mw)/2);

            cur.getkey();

            using namespace std::chrono_literals;
            if (timer_anim > 1s) {
             //   cur.swap_pair();
            //    cur.say_hello();

                //if (flipped) {
                //    cn.transeg(-7.5, -1.0, 1.0)
                //      .save();
                //    flipped = false;
                //} else {
                //    cn.sine()
                //      .save();
                //    flipped = true;
                //}

                cn.sine(++sawl).save();
                cn2 = cs.get(cn.id);
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
