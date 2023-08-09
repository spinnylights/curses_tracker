#include "pulse_train.hpp"

#include <cmath>

PulseTrain::PulseTrain(ticks nrte)
    : rte {nrte}
{}

PulseTrain::PulseTrain(time_f nrte)
    : PulseTrain(std::chrono::round<ticks>(nrte))
{}

void PulseTrain::update(ticks interv)
{
    auto memo = pos;
    pos %= rte;
    if (pos < memo) {
        on = true;
    } else {
        on = false;
    }
    pos += interv;
}

void PulseTrain::update(time_f interv)
{
    update(std::chrono::round<ticks>(interv));
}

bool PulseTrain::get() const
{
    return on;
}

void PulseTrain::rate(ticks nrte)
{
    rte = nrte;
}

void PulseTrain::rate(time_f nrte)
{
    rate(std::chrono::round<ticks>(nrte));
}
