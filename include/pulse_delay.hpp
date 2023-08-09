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

    bool get(bool sig, time_f t);
    bool get(bool sig, time_f::rep t) { return get(sig, time_f(t)); }

    time_f length() const { return time_f(len); }
    void   length(time_f nrate) { len = nrate; }

private:
    std::list<time_f> arrivals;
    time_f len;
    time_f last_time = time_f::zero();
    time_f elapsed = time_f::zero();
    bool on = false;
};

#endif
