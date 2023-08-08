#include "pulse_train.hpp"

#include <cmath>

PulseTrain::PulseTrain(time_f nrte)
    : rte {std::chrono::round<ticks>(nrte)}
{}

void PulseTrain::update(time_f interv)
{
    pos += std::chrono::round<ticks>(interv);
    auto memo = pos;
    pos %= rte;
    if (pos < memo) {
        on = true;
    } else {
        on = false;
    }
}

bool PulseTrain::get() const
{
    return on;
}

void PulseTrain::rate(time_f nrte)
{
    rte = std::chrono::round<ticks>(nrte);
}
