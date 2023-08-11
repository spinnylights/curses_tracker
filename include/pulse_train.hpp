#ifndef d59c7790531e4ae3bda692dcd33bb9f5
#define d59c7790531e4ae3bda692dcd33bb9f5

#include "defs.hpp"

class PulseTrain {
public:
    PulseTrain(ticks rate);

    void update(ticks time);

    bool get() const;

    ticks rate() const { return rte; }
    void  rate(ticks nrate);

private:
    ticks rte;
    bool on;
};

#endif
