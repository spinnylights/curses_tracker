#include "pulse_delay.hpp"

#include <doctest.h>

TEST_CASE("pulse delay (static length)") {
    auto len = ticks_per_sec;
    PulseDelay p {len};

    // emits only when len has passed
    auto start = ticks::zero();
    p.update(true,  start);
    CHECK(p.get() == false);
    p.update(false, start + len/2);
    CHECK(p.get() == false);
    p.update(false, start + len);
    CHECK(p.get() == true);
    p.update(false, start + len);
    CHECK(p.get() == false);
    p.update(false, start + len*2);
    CHECK(p.get() == false);

    // emits any time after len has passed
    start = len*3;
    p.update(true,  start);
    CHECK(p.get() == false);
    p.update(false, start + tics(1.1));
    CHECK(p.get() == true);

    // emits multiple pulses
    start = len*5;
    p.update(true,  start);
    CHECK(p.get() == false);
    p.update(true,  start + ticks_per_sec/2);
    CHECK(p.get() == false);
    p.update(false, start + ticks_per_sec);
    CHECK(p.get() == true);
    p.update(true,  start + ticks_per_sec*3/2);
    CHECK(p.get() == true);
    p.update(false, start + ticks_per_sec*2);
    CHECK(p.get() == false);
    p.update(false, start + ticks_per_sec*5/2);
    CHECK(p.get() == true);
}

TEST_CASE("pulse delay (changing length)") {
    auto len = tics(1);
    PulseDelay p {len};

    auto start = ticks::zero();

    p.update(true,  start);
    CHECK(p.get() == false);
    p.update(false, start + len/2);
    CHECK(p.get() == false);
    p.length(len*3/4);
    p.update(false, start + len*3/4);
    CHECK(p.get() == true);
    p.update(false, start + len);
    CHECK(p.get() == false);
}
