#ifndef r579df097cfd4a10aa8d4571ca909b2c
#define r579df097cfd4a10aa8d4571ca909b2c

#include <list>

class Delay {
public:
    typedef double sample;
    // buffer should maybe be its own class, or maybe this class should be
    // buffer :P
    typedef std::list<sample>      buffer;
    typedef buffer::size_type      buffer_ndx;
    typedef buffer::const_iterator read_head;

    Delay(buffer_ndx len, buffer_ndx rate);

    sample pushpop(sample);

    void adjrate(buffer_ndx nrate) { rate = nrate; }

private:
    buffer     samps {};
    buffer_ndx len;
    buffer_ndx rate;
    read_head  rh;
};

#endif
