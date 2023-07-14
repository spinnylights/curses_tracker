#ifndef Gc8dac77ce0245418b9092442d2fc608
#define Gc8dac77ce0245418b9092442d2fc608

#include <chrono>

class Curses {
public:
    Curses();

    ~Curses() noexcept;

    void main_loop();

private:
    using clock = std::chrono::steady_clock;
    clock::duration timer;

    void say_hello();

    int pair = 1;
};

#endif
