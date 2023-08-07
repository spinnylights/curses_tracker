#include "pulse_train.hpp"

#include <cmath>

PulseTrain::PulseTrain(time_f::rep nsamp_len, time_f nrate)
    : samp_len {nsamp_len},
      rte {nrate.count()}
{}

bool PulseTrain::get(time_f time) const
{
    time_f::rep mod_time = std::fmod(time.count(), rte);
    if (mod_time <= samp_len) {
        return true;
    } else {
        return false;
    }
}
