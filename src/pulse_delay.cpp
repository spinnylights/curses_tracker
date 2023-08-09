#include "pulse_delay.hpp"

#include <cmath>

PulseDelay::PulseDelay(time_f nlen)
    : len {nlen}
{}

void PulseDelay::update(bool sig, time_f time)
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
    //if (sig) {
    //    elapsed = time_f::zero();
    //    last_time = time_f::zero();
    //    on = true;
    //} else {
    //    elapsed += time - last_time;
    //    last_time = time;
    //}

    //if (on && elapsed > len) {
    //    on = false;
    //    return true;
    //} else {
    //    return false;
    //}
}
