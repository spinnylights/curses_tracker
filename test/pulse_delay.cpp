#include "pulse_delay.hpp"

#include <doctest.h>

TEST_CASE("pulse delay (static length)") {
    time_f len {1};
    PulseDelay p {len};

    // emits only when len has passed
    auto start = time_f::zero();
    CHECK(p.get(true,  start)         == false);
    CHECK(p.get(false, start + len/2) == false);
    CHECK(p.get(false, start + len)   == true);
    CHECK(p.get(false, start + len)   == false);
    CHECK(p.get(false, start + len*2) == false);

    // emits any time after len has passed
    start = len*3;
    CHECK(p.get(true,  start)               == false);
    CHECK(p.get(false, start + time_f(1.1)) == true);

    // emits multiple pulses
    start = len*5;
    CHECK(p.get(true,  start)               == false);
    CHECK(p.get(true,  start + time_f(0.5)) == false);
    CHECK(p.get(false, start + time_f(1.0)) == true);
    CHECK(p.get(true,  start + time_f(1.5)) == true);
    CHECK(p.get(false, start + time_f(2.0)) == false);
    CHECK(p.get(false, start + time_f(2.5)) == true);
}

TEST_CASE("pulse delay (changing length)") {
    time_f len {1};
    PulseDelay p {len};

    auto start = time_f::zero();

    CHECK(p.get(true,  start)           == false);
    CHECK(p.get(false, start + len/2)   == false);
    p.length(len*3/4);
    CHECK(p.get(false, start + len*3/4) == true);
    CHECK(p.get(false, start + len)     == false);
}
