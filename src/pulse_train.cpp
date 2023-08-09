#include "pulse_train.hpp"

#include <cmath>

PulseTrain::PulseTrain(time_f sample_rate, time_f nrte)
    : sampr {sample_rate},
      rte {nrte}
{}

bool PulseTrain::get(time_f time)
{
    time_f::rep mod_time = std::fmod(time.count(), rte.count());
    if (mod_time <= sampr.count()) {
    //if (time >= next) {
        next = time + rte;
        return true;
    } else {
        return false;
    }
}

void PulseTrain::rate(time_f nrte, time_f time)
{
    rte = nrte;
    next = time + rte;
}
