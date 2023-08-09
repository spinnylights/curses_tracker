#include "pulse_train.hpp"

#include <doctest.h>

TEST_CASE("pulse train") {
    auto rate = tics(1.0);
    PulseTrain pt {rate};

    pt.update(rate/2);
    CHECK(pt.get() == false);
    pt.update(rate);
    CHECK(pt.get() == true);
    pt.update(rate*7/4);
    CHECK(pt.get() == false);
    pt.update(rate*2);
    CHECK(pt.get() == true);
}
