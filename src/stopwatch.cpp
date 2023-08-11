#include "stopwatch.hpp"

void Stopwatch::update()
{
    pos += ticks_per_samp;
}

void Stopwatch::reset()
{
    pos = ticks(0);
}
