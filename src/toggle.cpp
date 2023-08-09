#include "toggle.hpp"

void Toggle::update(bool tog)
{
    if (tog) {
        pink_side = !pink_side;
    }
}
