#include "toggle.hpp"

#include <doctest.h>

TEST_CASE("basic toggle switching") {
    Toggle<sample> t;

    sample green = 1.0;
    sample pink  = -1.0;

    CHECK(t.get(green, pink) == green);

    t.update(true);
    CHECK(t.get(green, pink) == pink);
    t.update(false);
    CHECK(t.get(green, pink) == pink);

    t.update(true);
    CHECK(t.get(green, pink) == green);
    t.update(false);
    CHECK(t.get(green, pink) == green);
}

TEST_CASE("toggle logic") {
    Toggle<bool> t1;
    Toggle<bool> t2;

    SUBCASE("AND") {
        //bool green = false;
        //bool pink  = false;

        auto t1_out = t1.get(false, true);
        t2.get(false, true);
    }
}
