#include "pulse_delay.hpp"

#include <cmath>

PulseDelay::PulseDelay(ticks nlen)
    : len {nlen}
{}

void PulseDelay::update(bool sig, ticks time)
{
    if (sig) {
        arrivals.push_back(time);
    }

    on = false;

    for (auto i = arrivals.begin(); i != arrivals.end();) {
        if (time >= *i + len) {
            i = arrivals.erase(i);
            on = true;
        } else {
            ++i;
        }
    }
}

bool PulseDelay::get() const
{
    return on;
}
