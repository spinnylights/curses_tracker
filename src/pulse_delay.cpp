#include "pulse_delay.hpp"

#include <cmath>

PulseDelay::PulseDelay(time_f nlen)
    : len {std::chrono::round<ticks>(nlen)}
{}

void PulseDelay::update(bool sig, ticks interv)
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

    time += interv;
}

bool PulseDelay::get()
{
    return on;
    //bool out = false;

    ////auto out_cnt = arrivals.remove_if([len, time](time_f t){
    ////                                  return time > t + len; });

    //for (auto i = arrivals.begin(); i != arrivals.end();) {
    //    if (time >= *i + len) {
    //        i = arrivals.erase(i);
    //        out = true;
    //    } else {
    //        ++i;
    //    }
    //}

    //return out;
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
