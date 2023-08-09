#include "pulse_train.hpp"

#include <doctest.h>

TEST_CASE("pulse train") {
    time_f sampr {1/48000.0};
    time_f rate {1};
    PulseTrain pt {sampr, rate};

    CHECK(pt.get(rate/2)   == false);
    CHECK(pt.get(rate)     == true);
    CHECK(pt.get(rate*7/4) == false);
    CHECK(pt.get(rate*2)   == true);
}
