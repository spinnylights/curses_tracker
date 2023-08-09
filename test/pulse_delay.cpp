#include "pulse_delay.hpp"

#include <doctest.h>

TEST_CASE("pulse delay (static length)") {
    time_f len {1};
    PulseDelay p {len};

    // emits only when len has passed
    auto start = time_f::zero();
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
    p.update(false, start + time_f(1.1));
    CHECK(p.get() == true);

    // emits multiple pulses
    start = len*5;
    p.update(true,  start);
    CHECK(p.get() == false);
    p.update(true,  start + time_f(0.5));
    CHECK(p.get() == false);
    p.update(false, start + time_f(1.0));
    CHECK(p.get() == true);
    p.update(true,  start + time_f(1.5));
    CHECK(p.get() == true);
    p.update(false, start + time_f(2.0));
    CHECK(p.get() == false);
    p.update(false, start + time_f(2.5));
    CHECK(p.get() == true);
}

TEST_CASE("pulse delay (changing length)") {
    time_f len {1};
    PulseDelay p {len};

    auto start = time_f::zero();

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
