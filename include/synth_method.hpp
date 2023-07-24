#ifndef Qd932e98ea1b4422b0336f360e3b4ba0
#define Qd932e98ea1b4422b0336f360e3b4ba0

#include <cstdint>
#include <cmath>

class SynthMethod {
public:
    ///* override one of the two of these */
    //virtual void pitch(double) { };
    //virtual void trigger(uint64_t) { };

    virtual void delta(double dt) { d = dt; };
    virtual double delta() const { return d; };

    virtual double samp() const { return smp; };

    virtual void update() = 0;
    virtual void update(double delta_t)
    {
        d += delta_t;
        update();
    };

protected:
    virtual void samp(double nsmp) { smp = nsmp; }

private:
    double d = 0.0;
    double smp = 0.0;
};

class PitchSynth : public SynthMethod {
public:
    virtual void pitch(double infreq) { hz = infreq; }
    double pitch() const { return hz; }
    virtual void freq(double infreq) { pitch(infreq); }
    double freq() const { return pitch(); }

private:
    double hz;
};

class TrigSynth : public SynthMethod {
    virtual void trig(uint64_t) = 0;
};

/*
 * SimpleSine s;
 * double smp = 0.0;
 * double dt  = 0.0
 *
 * s.pitch(440.0);
 * s.update();
 * smp = s.samp();
 * s.update(dt += time_interval);
 * smp = s.samp();
 * s.update(dt += time_interval);
 * smp = s.samp();
 * ...
 */
class SimpleSine : public PitchSynth {
public:
    void update()
    {
        static constexpr double tau = 2*M_PI;
        samp(std::sin(tau*freq()*delta()));
    }
};

#endif
