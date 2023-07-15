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

private:
    NoteView nv;
    struct winsize ws {};
    int pair = 1;
};

#endif
