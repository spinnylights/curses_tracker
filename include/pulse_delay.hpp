#ifndef f4c08f8e266549fa9410a189ce31464f
#define f4c08f8e266549fa9410a189ce31464f

#include "defs.hpp"

#include <list>


class PulseDelay {
public:
    PulseDelay(ticks rate);

    void update(bool sig, ticks t);

    bool get() const;

    ticks length() const { return len; }
    void  length(ticks nrate) { len = nrate; }

private:
    std::list<ticks> arrivals;
    ticks len;
    ticks last_time = ticks::zero();
    ticks elapsed = ticks::zero();
    bool on = false;
};

#endif
