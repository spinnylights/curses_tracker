#include "pulse_train.hpp"

#include <doctest.h>

TEST_CASE("pulse train") {
    time_f sampr {1/48000.0};
    time_f rate {1};
    PulseTrain pt {sampr, rate};

    pt.update(rate/2);
    CHECK(pt.get() == false);
    pt.update(rate/2);
    CHECK(pt.get() == true);
    pt.update(rate/2);
    CHECK(pt.get() == false);
    pt.update(rate/2);
    CHECK(pt.get() == true);
}
