#ifndef jf0b0b9263a840b39f83ee6270408a9a
#define jf0b0b9263a840b39f83ee6270408a9a

#include "synth_method.hpp"

#include <chrono>
#include <memory>

/* an instrument has:
 *     * a synthesis method
 *     * settings for its parameters
 *     * its own parameters, which can influence the settings
 *         * these parameters can optionally be made available to the score
 *
 * instruments generate sound
 *
 * they have a method `activate(freq)` or `activate(nat_number)`, which starts a
 * sound-generation event
 *     - it probably zeroes out and then starts updating a timer in some sense
 *     - it should have a method `samp(secs)` where you pass the number of
 *       elapsed seconds since the last `samp(secs)` and it advances the timer
 *       that amount of seconds and returns a new (float32?) sample
 *
 * ultimately the thing that will use the `freq` or `nat_number` is the
 * synthesis method
 *
 * synthesis method will be an abstract class…i'll go sketch it out
 */

class Instrument {
public:
    virtual void restart() { timer = clock::duration::zero(); }

    float samp(double dt)
    {
        std::chrono::duration<double> dtdur {dt};
        timer += std::chrono::duration_cast<clock::duration>(dtdur);
        return _samp();
    } 

    virtual float _samp() = 0; 

    double t() const
    {
        return std::chrono::duration<double>(timer).count();
    }

protected:
    using clock = std::chrono::steady_clock;
private:
    clock::duration timer = clock::duration::zero();
};

class PitchInstr : public Instrument {
public:
    PitchInstr(std::unique_ptr<PitchSynth>&& insynth)
        : synth {std::move(insynth)}
    {};

    float _samp()
    {
        return 0.0;
    }

private:
    std::unique_ptr<PitchSynth> synth;
};

class TrigInstr : public Instrument {

};

#endif
