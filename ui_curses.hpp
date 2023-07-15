#ifndef Gc8dac77ce0245418b9092442d2fc608
#define Gc8dac77ce0245418b9092442d2fc608

#include "event.hpp"

#include <chrono>

class Curses {
public:
    Curses();

    ~Curses() noexcept;

    void main_loop();
    void say_hello();
    void swap_pair();

    void getkey();

private:
    int pair = 1;

    NoteView nv;
};

#endif
