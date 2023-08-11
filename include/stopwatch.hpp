#ifndef Qe3cc40c5e044306802683cc31ce21b2
#define Qe3cc40c5e044306802683cc31ce21b2

#include "defs.hpp"

class Stopwatch {
public:
    void update();

    void reset();
    void reset(bool b) { if (b) reset(); }

    ticks  get()  const { return pos; }
    time_f getf() const { return time_f(pos); }

    operator ticks()  const { return get(); }
    operator time_f() const { return getf(); }

    double operator*(double x) { return getf().count() * x; }
    bool   operator<(double x) { return getf().count() < x; }
    bool   operator>(double x) { return getf().count() > x; }

private:
    ticks pos {0};
};

#endif
