#include "pulse_train.hpp"

#include <cmath>

PulseTrain::PulseTrain(ticks nrte)
    : rte {nrte}
{}

void PulseTrain::update(ticks time)
{
    ticks mod_time = time % rte;
    if (mod_time <= ticks_per_samp) {
        on = true;
    } else {
        on = false;
    }
}

bool PulseTrain::get() const
{
    return on;
}

void PulseTrain::rate(ticks nrte)
{
    rte = nrte;
}
