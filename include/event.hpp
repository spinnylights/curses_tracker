#ifndef b3bdd72cefb34aed94103c3411dfea65
#define b3bdd72cefb34aed94103c3411dfea65

#include "instrument.hpp"

class Event {
public:
    typedef unsigned long long ticks_t;

    virtual ~Event() = default;

    virtual ticks_t pos() const { return position; }
    virtual ticks_t len() const { return dur; }

    virtual void sr(double sample_rate) { sampr = sample_rate; }
    virtual double tick_len() const { return 1.0/sampr; }

private:
    double sampr = 48000;
    ticks_t position = 0;
    ticks_t dur = 0;
};

class PitchEvent : public Event {
};

class TrigEvent : public Event {
};

#endif
