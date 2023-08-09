#ifndef f4c08f8e266549fa9410a189ce31464f
#define f4c08f8e266549fa9410a189ce31464f

#include "defs.hpp"

#include <list>


class PulseDelay {
public:
    PulseDelay(time_f rate);
    PulseDelay(time_f::rep rate)
        : PulseDelay(time_f(rate))
    {};

    void update(bool sig, ticks interv);
    void update(bool sig, time_f interv)
    {
        update(sig, std::chrono::round<ticks>(interv));
    }

    bool get();

    time_f length() const { return time_f(len); }
    void   length(time_f nrate) { len = std::chrono::round<ticks>(nrate); }

private:
    std::list<ticks> arrivals;
    //time_f len;
    //time_f last_time = time_f::zero();
    //time_f elapsed = time_f::zero();
    ticks len;
    ticks last_time = ticks::zero();
    ticks time = ticks::zero();
    bool on = false;
};

#endif
