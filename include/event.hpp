#ifndef b3bdd72cefb34aed94103c3411dfea65
#define b3bdd72cefb34aed94103c3411dfea65

#include "instrument.hpp"

/*
 * an event consists of:
 *     * a timeline position
 *     * either a note or a natural number
 *     * an instrument
 *     * various parameter settings
 *
 *  the instrument determines note vs. natural number, and if natural number,
 *  the instrument determines the bounds
 *
 *  the instrument also influences the parameters
 *
 *  maybe i should have this be a template, like for "float events" and
 *  "unsigned events" or something
 *
 *  or, i might have pitchevent and trigevent
 *
 *  2/4
 *  000 8.43 I AOR
 *  001 8.39    -
 *
 *  4/8
 *  000 8.43 I AOR
 *  001
 *  002 8.39    -
 *  003
 *
 *  <0.00> len 0.25, pitched instr, 8 oct, 43 pc, params: amp [0.8],
 *             offset [0.1], reverb [0.6 etc. etc.]
 *  <0.50> len smthng::to_end, pitched instr, 8 oct, 39 pc, params:
 *             amp [unchanged], offset [default (i.e. 0)], reverb [unchanged]
 *
 * therefore an event has a start time, a duration, a pitch or trig instrument,
 *     a note if pitched and an index if trig, and a set (list?) of params
 */
class Event {
public:
    typedef unsigned long long ticks_t;

    virtual ~Event() = default;

    virtual ticks_t pos() const { return position; }
    virtual ticks_t len() const { return dur; }

    /* these should probably go somewhere else
     * maybe a sample rate class
     */
    virtual void sr(double sample_rate) { sampr = sample_rate; }
    virtual double tick_len() const { return 1.0/sampr; }

private:
    double sampr = 48000;
    ticks_t position = 0;
    ticks_t dur = 0;
};

class PitchEvent : public Event {
    //PitchEvent()
};

class TrigEvent : public Event {

};

#endif
