#ifndef d59c7790531e4ae3bda692dcd33bb9f5
#define d59c7790531e4ae3bda692dcd33bb9f5

#include "defs.hpp"

class PulseTrain {
public:
    PulseTrain(time_f sample_rate, time_f rate);

    void update(time_f time);

    bool get() const;

    time_f rate() const { return rte; }
    void   rate(time_f nrate);

private:
    time_f sampr;
    time_f rte;
    bool on;
};

#endif
