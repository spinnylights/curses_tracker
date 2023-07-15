#ifndef Gc8dac77ce0245418b9092442d2fc608
#define Gc8dac77ce0245418b9092442d2fc608

#include "event.hpp"

#include <sys/ioctl.h>

#include <chrono>

class Curses {
public:
    Curses();

    ~Curses() noexcept;

    void main_loop();
    void say_hello();
    void swap_pair();

    void getkey();
    void update_winsz();

    unsigned short w() { return ws.ws_col; }
    unsigned short h() { return ws.ws_row; }

private:
    NoteView nv;
    struct winsize ws {};
    int pair = 1;
};

#endif
