#include "pulse_train.hpp"

#include <cmath>

PulseTrain::PulseTrain(time_f sample_rate, time_f nrte)
    : sampr {sample_rate},
      rte {nrte}
{}

void PulseTrain::update(time_f time)
{
    time_f::rep mod_time = std::fmod(time.count(), rte.count());
    if (mod_time <= sampr.count()) {
    //if (time >= next) {
        next = time + rte;
        on = true;
    } else {
        on = false;
    }
}

bool PulseTrain::get() const
{
    return on;
}

void PulseTrain::rate(time_f nrte, time_f time)
{
    rte = nrte;
    next = time + rte;
}
