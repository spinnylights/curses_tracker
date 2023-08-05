# `curses_tracker`

I'm going to give this a proper name eventually, I'm just not
sure what yet. This is going to be a
[curses](https://en.wikipedia.org/wiki/Curses_(programming_library))-based
tracker-style sequencer with built-in instruments and effects.

Here's what it can do audio-wise right now. It has algorithms for
rendering various curves you can describe with a li'l DSL—namely,
curves going from one point to another at varying rates,
sinusoids, and a series-based interface for making band-limited
wavetables which you can use for wavetable synthesis. You can
seek through these curves/tables using linear interpolation with
various sticking or looping modes. There's also a very rough
delay that needs various refinements which I slapped together
rapidly out of raw desperation a few hours ago. For pitch, it
understands notes in
[53-EDO](https://en.wikipedia.org/wiki/53_equal_temperament),
including fractional notes, and supports basic arithmetic on
them. It also has a sense of time, both in fractional terms and
as discrete ticks. It can output audio via SDL.

Its coolest graphical feature so far is probably that it can plot
any of the curves/tables using the curses character set at
whichever width and height you choose. In the current state of
the program, it uses this facility to highlight which of two
wavetables is currently driving the wavetable synthesizer as it
plays music using the above-described audio features. In the
background you can see it render a pretend musical score, and in
the top left corner you can see a "cursor pulse" effect which I'm
planning to use to highlight the current-and-recently-played rows
when I get score-based playback working.

I mainly wanted to commit and upload the code somewhere now
because I'm extremely pleased with the current behavior of the
program (:D) and I started feeling sort of irrationally worried
like "what if the apartment building caught fire suddenly and all
my hardware melted and I lost my work?" so I decided to put it up
here and on my web server to give myself peace of mind. ;^^

## Build instructions

If you have curses and SDL2 installed, I think it should be as
simple as `cd`ing into the checked-out repo and doing:

```bash
mkdir build && cd build
autoreconf -i
../configure --srcdir=..
make
```

Then you can run it from the build directory with
`./curses_tracker`. That's all I've been doing so far.

If you're on Windows, you should be able to build and run this in
[Cygwin](https://www.cygwin.com/). When you're installing Cygwin,
make sure the packages for Git, SDL, curses, GCC, Autoconf, and
Automake are selected. Once you get it running and you're at the
command line, you can run `git clone
https://github.com/spinnylights/curses_tracker` and then `cd
curses_tracker` and then you can use the instructions above.

I can't make any promises this will actually run on anyone else's
computer at this point, though. It's still in a very early phase
of development and I haven't tested it on anything else but my
local workstation yet. Now that it's online I'll probably end up
trying it out in some \*nix VMs and on Lily's Windows machine.

## License

Copyright 2023 Zoë Sparks (spinnylights)

This program is free software: you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License (LICENSE.md) for more details.
