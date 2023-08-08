#include "toggle.hpp"

sample Toggle::get(bool tog, sample green, sample pink)
{
    if (tog) {
        pink_side = !pink_side;
    }

    if (pink_side) {
        return pink;
    } else {
        return green;
    }
}
