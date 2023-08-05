#include "delay.hpp"

Delay::Delay(buffer_ndx nlen, buffer_ndx nrate)
    : len {nlen},
      rate {nrate}
{
    for (buffer_ndx i = 0; i < len; ++ i) {
        samps.push_front(0.0);
    }

    rh = samps.cbegin();
}

Delay::sample Delay::pushpop(sample s)
{
    auto out = *rh;
    
    for (buffer_ndx i = 0; i < rate; ++i) {
        ++rh;
        if (rh == samps.cend()) {
            rh = samps.cbegin();
        }
    }

    if (rh == samps.cbegin()) {
        ++rh;
    }

    samps.pop_front();
    samps.push_back(s);

    return out;
}
