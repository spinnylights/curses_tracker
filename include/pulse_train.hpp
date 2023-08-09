#ifndef d59c7790531e4ae3bda692dcd33bb9f5
#define d59c7790531e4ae3bda692dcd33bb9f5

#include "defs.hpp"

#include <chrono>

class PulseTrain {
public:
    PulseTrain(ticks rate);
    PulseTrain(time_f rate);

    void update(ticks interv);
    void update(time_f interv);

    bool get() const;

    ticks rate() const { return rte; }
    void  rate(ticks nrate);
    void  rate(time_f nrate);

private:
    // the way this works is like...
    //     uint64_t and ratio<1,1 << 0> yields
    //         resolution of 1/(1 << 0) s (1 s)
    //         max period of [(1 << (64 - 0) - 1] s (approx. 18.45 Es, past the
    //                                               current age of theu
    //                                               universe)
    //         highest speed: (1 << 0) Hz
    //         lowest speed:  1/[(1 << (64 - 0)) - 1] Hz
    //
    //     uint64_t and ratio<1,1 << 1> yields
    //         resolution of 1/(1 << 1) s (0.5 s)
    //         max period of [(1 << (64 - 1) - 1] s (approx. 9.22 Es, universe
    //                                               is still younger)
    //         highest speed: (1 << 1) Hz
    //         lowest speed:  approx. 1/[(1 << (64 - 1)) - 1] Hz
    //
    //     uint64_t and ratio<1,1 << 32> yields
    //         resolution of 1/(1 << 32) s (approx. 233 ps, around the length of
    //                                      a clock cycle of my CPU)
    //         max period of [(1 << (64 - 32)) - 1] s (approx. 136 years)
    //         highest speed: (1 << 32) Hz (approx. 4.3 Ghz)
    //         lowest speed:  approx. 1/[(1 << (64 - 32)) - 1] Hz
    //
    //     uint64_t and ratio<1,1 << 50> yields
    //         resolution of 1/(1 << 50) s (approx. 888 as, a little longer
    //                                      than electron transfer)
    //         max period of [(1 << (64 - 50)) - 1] s (approx. 4.5 hours)
    //         highest speed: (1 << 32) Hz (approx. 4.3 Ghz)
    //         lowest speed:  approx. 1/[(1 << (64 - 32)) - 1] Hz
    //
    // well, that was fun :P why don't we do ratio<1, 1 << 32>? that's nice and
    // round and can accomodate rates both at the edge of what home computers
    // can reasonable handle and longer than any known human lifetime (both as
    // of 2023)
    ticks pos {0};
    ticks rte;
    bool on = false;
};

#endif
