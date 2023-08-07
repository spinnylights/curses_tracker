#ifndef d59c7790531e4ae3bda692dcd33bb9f5
#define d59c7790531e4ae3bda692dcd33bb9f5

#include "defs.hpp"

class PulseTrain {
public:
    PulseTrain(time_f::rep samp_len, time_f rate);

    bool get(time_f) const;

    time_f rate() const { return time_f(rte); }
    void   rate(time_f nrate) { rte = nrate.count(); }

private:
    time_f::rep samp_len;
    time_f::rep rte;
};

#endif
