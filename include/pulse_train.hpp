#ifndef d59c7790531e4ae3bda692dcd33bb9f5
#define d59c7790531e4ae3bda692dcd33bb9f5

#include "defs.hpp"

class PulseTrain {
public:
    PulseTrain(time_f sample_rate, time_f rate);

    bool get(time_f);

    time_f rate() const { return rte; }
    void   rate(time_f nrate, time_f time);

private:
    time_f sampr;
    time_f rte;
    time_f next = rte;
};

#endif
