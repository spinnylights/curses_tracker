#include "pulse_train.hpp"

#include <doctest.h>

TEST_CASE("pulse train") {
    time_f rate {0.0009765625};
    PulseTrain pt {rate};

    for (int i = 0; i < 16; ++i) {
        pt.update(rate/2);
        if (i % 2 == 0) {
            CHECK(pt.get() == false);
        } else {
            CHECK(pt.get() == true);
        }
    }
}
