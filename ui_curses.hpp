#ifndef Gc8dac77ce0245418b9092442d2fc608
#define Gc8dac77ce0245418b9092442d2fc608

#include "event.hpp"
#include "audio_sdl.hpp"

#include <sys/ioctl.h>

#include <chrono>
#include <curses.h>

class Curses {
public:
    Curses();

    ~Curses() noexcept;

    void main_loop();
    void say_hello(const Audio& aud);
    void swap_pair();

    void getkey();
    void update_winsz();

    unsigned short w() { return ws.ws_col; }
    unsigned short h() { return ws.ws_row; }

    void w(unsigned short n) { ws.ws_col = n; }
    void h(unsigned short n) { ws.ws_row = n; }

private:

private:
    WINDOW* win = nullptr;
    struct winsize ws {};
    int pair = 1;
    int key = ' ';
};

#endif
