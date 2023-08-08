#include "pulse_train.hpp"

#include <cmath>

PulseTrain::PulseTrain(time_f sample_rate, time_f nrte)
    : sampr {sample_rate},
      rte {nrte}
{}

void PulseTrain::update(time_f interv)
{
    time += interv;
    if (get()) {
        next = time + rte;
    }
}

bool PulseTrain::get() const
{
    time_f::rep mod_time = std::fmod(time.count(), rte.count());
    if (mod_time <= sampr.count()) {
    //if (time >= next) {
        return true;
    } else {
        return false;
    }
}

void PulseTrain::rate(time_f nrte)
{
    rte = nrte;
    next = time + rte;
}
