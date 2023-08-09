#include "pulse_delay.hpp"

#include <doctest.h>

TEST_CASE("pulse delay (static length)") {
    time_f lenf {1};
    ticks len {std::chrono::round<ticks>(lenf)};
    PulseDelay p {len};

    // emits only when len has passed
    p.update(true, ticks(0));
    CHECK(p.get() == false);
    p.update(false, len/2);
    CHECK(p.get() == false);
    p.update(false, len/2);
    CHECK(p.get() == true);
    p.update(false, len/2);
    CHECK(p.get() == false);
    p.update(false, len);
    CHECK(p.get() == false);

    //// emits any time after len has passed
    p.update(true, len*3);
    CHECK(p.get() == false);
    p.update(false, std::chrono::round<ticks>(time_f(1.1)));
    CHECK(p.get() == true);

    // emits multiple pulses
    p.update(true, len*5);
    CHECK(p.get() == false);
    p.update(true, time_f(0.5));
    CHECK(p.get() == false);
    p.update(false, time_f(0.5));
    CHECK(p.get() == true);
    p.update(false, time_f(0.25));
    CHECK(p.get() == false);
    p.update(false, time_f(0.25));
    CHECK(p.get() == true);
    p.update(false, time_f(0.25));
    CHECK(p.get() == false);
    p.update(false, time_f(1.5));
    CHECK(p.get() == false);
}

TEST_CASE("pulse delay (changing length)") {
    time_f len {1};
    PulseDelay p {len};

    auto start = time_f::zero();

    p.update(true, ticks(0));
    CHECK(p.get() == false);
    p.update(true, len/2);
    CHECK(p.get() == false);
    p.length(len*3/4);
    p.update(true, len*1/4);
    CHECK(p.get() == true);
    p.update(true, len*1/4);
    CHECK(p.get() == false);
}
