# How `08d8c60a` works
8/5/23

I want to break down for myself how the application at commit
`08d8c60ad8aab41c8c24468f5744681919e1c211` works, in a
speech-style, abstract fashion. My hope is that by doing this I
can clarify for myself what I was trying to express to the
program when I wrote the code in `Synth::sample()` and some other
related places (`sample.hpp`, `main.cpp`)—the code in that commit
that is responsible for the most distinctive aspects of the
behavior the user hears and sees when they run the program. I
guess, what I really mean by "most distinctive" is the behavior
that feels like it is part of a specific audiovisual piece more
than part of the underlying machinery. Ultimately, I'd like to
extract an interface to `Synth` from this, and lay some
groundwork for the "piece" data structure.

## Analysis

So, first, we have the two wavetables, which appear at
`main.cpp:64`:

```cpp
"soidser  1.10  2.3 0.02" // 'low'
"soidser 10.75 10.0 0.52" // 'high'
```

The "low" wavetable is used for the "purple chord" which plays
when the left side of the screen is highlighted. The "high"
wavetable is used for the "orange chord" which plays when the
right side of the screen is highlighted.

Of course, in truth, neither of these tables is intrinsically
high or low in pitch. The reason for the names is that I used the
"low" wavetable for a chord with a lower melodic voice and a
darker overall harmonic character, and the "high" wavetable for a
chord with a higher melodic voice (by about a minor third) and
brighter overall harmonic character.

Of course, the "low" wavetable does have more energy in the low
harmonics than the "high" wavetable. Acoustically, though, I
think the differences between them aren't so easily characterized
that way. This is straying kind of off-topic though I guess.

So, anyway, we have these two wavetables. Here are the notes in
each chord, which are defined at the top of `synth.cpp`:

```cpp
low chord:

root 4.00
notes
  +0.00 // 4.00
  +0.10 // 4.10
  +0.44 // 4.44
  +1.09 // 5.09
```

```cpp
high chord:

root 3.30
notes
  +0.00 // 3.30
  +0.23 // 4.00
  +0.52 // 4.29
  +1.24 // 5.01
  +1.46 // 5.23
supplementary
  +1.03 // 4.33
  +1.09 // 4.39
  +1.49 // 5.26
```

The "supplementary" notes in the high chord are played at about
11% of the volume of the main notes (see `synth.cpp:183`). They
add a metallic color, through their relatively dissonant
relationships with certain notes in the main chord and relatively
consonant ones with others.

I want to make a brief few of notes about the UI design,
thinking about this.

First, we can see that I had the impulse to note both the
relative interval of each note with the root as well as their
absolute pitch. Both of these are useful and the program should
represent both. I'm not sure that this point if it should
represent them both at all times or if you should be able to
toggle each representation on and off; I'll have to see how I
feel down the line.

It would also be nice if there was an easy way to view relative
consonance and dissonance between one note in a chord and the
other notes, as well as the notes of nearby chords—maybe through
color. I think that would be a great aid to composition. In that
case, because it's all relative to one note, it would have to be
something that only turns on in specific cases—perhaps when the
cursor is over a particular note.

I could probably put the code for computing the normalized
dissonance of two notes in `Note`. That would be fun since I
already have a favorite algorithm for that.

Anyway, that's enough about the UI. Let's return to the program.

The notes and wavetables are brought together in `Synth`'s
constructor. `Synth()` right now takes descriptions in the curve
DSL for the "low" and "high" waveforms and makes small lookup
tables pairing curves and note frequencies for my convenience
later on in `Synth::sample()`. The code in `Synth()` that does
this is extremely messy with lots of duplication and is wasteful
of time and memory besides. :P It's standing in for me having a
mental model of how curves will actually be stored and accessed
in `Synth`, because I wanted to put off figuring that out at the
time in favor of just getting chord playback working in any
sense. I hope I'll make some progress on that mental model by the
end of this document.

Anyway, the other thing that happens in `Synth()` is that the
envlope and LFO tables get set up. This was the direct motivation
for the Curve wrap modes, to the extent that I added them going
back-and-forth between `Synth()` and `Curve`. They are:

```cpp
upramp 
  "segs"            // a straight line from 0 to 1
  sticky            // seeking is clamped at the endpoints

downupramp
  "segs 0 1 0"      // a straight line from 1 to 0
  ping-pong looping // seeking "reflects" at the endpoints

downramp
  "segs 0 1 0"
  sticky

sine
  "soid"            // a sine wave
  looping           // seeking wraps around at the endpoints
```

All of these seem like curves that it would be nice to have in
the global prefs for quick at-hand access. It's neat to see how
the looping mode does actually feel like an essential part of the
curve in terms of it being a kind of "signal-producing module."

There's one looping mode that doesn't appear here, "ping-pong
sticky," which reflects once at the right-side endpoint (`1.0`)
and then clamps at the left-side endpoint (`0.0`). This allows
you to easily make a symmetrical table—with double the
resolution—by defining half of the table and then seeking through
it at double the speed. I was planning to do that originally with
`downupramp`; I had actually intended it to be the transition
envelope between organ-style playback of the two chords. However,
after getting that working, I tried changing it to use ping-pong
looping and modulating its seek rate with an LFO, which is how
the envlopes in `08d8c60a` arrived at the character they have.

Anyway, that's all for `Synth()`. The next place of note is
`Synth::sample()`; the only change to `Audio::callback()` is that
it now accepts stereo samples from `Synth` (because
`Synth::sample()` now returns samples in stereo, as we'll get to).

`Synth::sample()` is where the bulk of everything happens, in
regards to the "specific piece" here. Just to quickly recollect,
what we're going into `Synth::sample()` with is:

* the wavetables and note frequencies for the two chords, "low"
  and "high"
* four other curves aside from the wavetables: three for envelope
  use, `upramp`, `downupramp`, and `downramp`, and one for LFO
  use, `sine`

At first in `Synth::sample()` we have a little routine for
waiting a number of samples before starting playback
(`synth.cpp:140`, `synth.hpp:61`). I added that to make it easier
to rig up audio and video recording outside of the program,
because the [JACK](https://jackaudio.org/) outputs for the
program don't appear until it starts up and makes full contact
with SDL and my recording script doesn't handle that gracefully
(:P). In the hypothetical tracker interface that would correlate
with adding a period of silence at the beginning somehow.

After that we have some very important parameters. One is
`lfo_rate`, which is 7/4 (1.75) s⁻¹ or 4/7 (0.57) Hz. (`sinv`
for s⁻¹ might be a nice unit suffix to add in the C++ code.)
Another is `ramp_time`, which is about [1/3 +
sin(`pos`·`lfo_rate`)/5] s. `ramp_time` ranges between 2/15
(0.13) s and 8/15 (0.53) s sinusoidally over a period of 7/4
s. It actually doesn't play as central a role as `ramp_time_2`
though, which is just 2·`ramp_time`. `ramp_time_2` ranges between
4/15 (0.27) s and 16/15 (1.07) s sinusoidally over a period of
7/4 s.

Let's express these as periods in a little table. Also, since in
practice `ramp_time_2` is used after the first second passes,
I'll just call it "env" and note later that the initial rising
envelope happens at twice its speed.

| period | median length (s) | min length (s) | max length (s) | varies at (Hz) |
| ------ | ----------------- | -------------- | -------------- | -------------- |
| lfo    | 7/4 (1.75)        | static         | static         | static         |
| env    | 2/3 (0.67)        | 4/15 (0.27)    | 16/15 (1.07)   | 4/7 (0.57)     |

"env" is used with both `upramp` and `downupramp` (I'll describe
how in full when we get there in the code).

Anyway, next (`synth.cpp:153`) we have another important
parameter: every three seconds, we switch the flag `in_cs2` on
for one second, then turn it off again. "cs2" in this case refers
to the high chord. However, in practice, even if `in_cs2 ==
true`, we may not actually be "in the high chord." As noted
shortly after (`synth.cpp:170`), once `in_cs2` is flipped on, we
wait until "env" time has elapsed to actually switch to the high
chord, and we wait again for "env" time to pass after `in_cs2` is
flipped off to switch back to the low chord. Of course, the
length of "env" varies over time, which is why the piece switches
back and forth between them at varying rates.

We switch `in_cs2` at integer values, in particular every 4n and
4n + 1 (except at first). 4n/7 becomes an integer multiple of 4
whenever n % 7 = 0.

```ruby
# hgh low                       4n/7 % 4
[  0,  1 ],   [ 0.0,                0.14285714285714324 ]
[  4,  5 ],   [ 0.5714285714285714, 0.7142857142857143  ]
[  8,  9 ],   [ 1.1428571428571428, 1.2857142857142858  ]
[ 12, 13 ],   [ 1.7142857142857142, 1.8571428571428572  ]
[ 16, 17 ],   [ 2.2857142857142856, 2.4285714285714284  ]
[ 20, 21 ],   [ 2.857142857142857,  3.0                 ]
[ 24, 25 ],   [ 3.4285714285714284, 3.5714285714285716  ]
[ 28, 29 ],   [ 0.0,                0.14285714285714324 ]
[ 32, 33 ],   [ 0.5714285714285712, 0.7142857142857144  ]
[ 36, 37 ],   [ 1.1428571428571432, 1.2857142857142856  ]
[ 40, 41 ],   [ 1.7142857142857144, 1.8571428571428568  ]
[ 44, 45 ],   [ 2.2857142857142856, 2.428571428571429   ]
[ 48, 49 ],   [ 2.8571428571428568, 3.0                 ]
[ 52, 53 ],   [ 3.428571428571429,  3.571428571428571   ]
[ 56, 57 ],   [ 0.0,                0.14285714285714235 ]
[ 60, 61 ],   [ 0.5714285714285712, 0.7142857142857135  ]
[ 64, 65 ],   [ 1.1428571428571423, 1.2857142857142865  ]
[ 68, 69 ],   [ 1.7142857142857135, 1.8571428571428577  ]
[ 72, 73 ],   [ 2.2857142857142865, 2.428571428571429   ]
[ 76, 77 ],   [ 2.8571428571428577, 3.0                 ]
[ 80, 81 ],   [ 3.428571428571429,  3.571428571428571   ]

# hgh low                   sin[2π(4n/7 % 4)]
[  0,  1 ], [  0.0,                 0.7818314824680298  ]
[  4,  5 ], [ -0.433883739117558,  -0.9749279121818236  ]
[  8,  9 ], [  0.7818314824680296,  0.9749279121818237  ]
[ 12, 13 ], [ -0.9749279121818235, -0.7818314824680301  ]
[ 16, 17 ], [  0.9749279121818237,  0.43388373911756023 ]
[ 20, 21 ], [ -0.7818314824680302,  0.0                 ]
[ 24, 25 ], [  0.4338837391175589, -0.43388373911755734 ]
[ 28, 29 ], [  0.0,                 0.7818314824680314  ]
[ 32, 33 ], [ -0.4338837391175564, -0.9749279121818237  ]
[ 36, 37 ], [  0.7818314824680312,  0.9749279121818237  ]
[ 40, 41 ], [ -0.9749279121818235, -0.7818314824680312  ]
[ 44, 45 ], [  0.9749279121818237,  0.43388373911755707 ]
[ 48, 49 ], [ -0.7818314824680325, -0.0                 ]
[ 52, 53 ], [  0.4338837391175589, -0.43388373911755734 ]
[ 56, 57 ], [  0.0,                 0.7818314824680278  ]
[ 60, 61 ], [ -0.4338837391175564, -0.9749279121818226  ]
[ 64, 65 ], [  0.7818314824680275,  0.9749279121818225  ]
[ 68, 69 ], [ -0.9749279121818223, -0.7818314824680279  ]
[ 72, 73 ], [  0.9749279121818225,  0.43388373911755707 ]
[ 76, 77 ], [ -0.781831482468028,   0.0                 ]
[ 80, 81 ], [  0.4338837391175589, -0.43388373911755734 ]
```

As we can see here, the pattern repeats at around every 30 s.—in
practice, for this piece it's at 32–33 s. because during the
first period we stay in the low chord and ramp up.

Anyway, after the high/low switching code, the audio for the low
or high chord is added into the output sample. This is where the
"supplementary" color notes are added into the high chord, if the
high chord is on.

After that, we finally get to the envelope. For the first second,
we stay in the low chord and scale the sample from 0 to 1 at half
of "env" (which ends up being a little over half a second or so).
This is so the piece fades in gradually. Otherwise, we take the
envelope from `downupramp` at the "env" rate and scale the sample
based on that.

Something I realize I haven't touched on—every time we switch
`in_cs2`, we reset a value called `env_pos` to 0. This increases
at the same rate as `pos`, but goes to 3 s, then to 1 s, then to
3 s, then to 1 s, etc. This is what we actually use to seek in
the envelopes, as opposed to `pos`. `env_pos` also gets set to 0
at the start of the shutdown routine, in order to start the down
ramp envelope from the beginning.

The existence of `env_pos` here is something of an artifact of
the fact that `Curve`s don't store an internal index you can
advance or anything. They just have `Curve::get()` which takes a
normalized index as an argument, so you have to keep track of the
index yourself. I'm still not entirely sure what I'm going to do
around that in design terms but I don't think it's going to be
like that forever.

Anyway, as it is, we never go further than 3 seconds into any
envelope. Since `downupramp` loops, though, and because the high
and low chords switch back and forth in a complicated pattern,
the envelope and the chords interact in an interesting way. This
does explain why everything lines back up once the chord
switching sequence repeats, since it also depends on multiples of
4.

Also, I just realized, I think there might be a bug in the
ping-pong looping code, because I tried advancing the index into
it just using `pos` and it didn't seem to reflect at 1.0. Also,
the program seems to crash if I pass `-pos`. So, there's still
some work to do with the `Curve::wrapm` stuff. I'll have to
figure out what's going on so I can preserve the distinctive
sound of this piece. :P

Anyway, that brings us to the end of the main "instrumental
synthesis" part. We scale the output by `0.9` and then send it on
to the delays, which I guess are the "effects" in this case.

There are three delays, `delay_1`, `delay_2`, and `delay_3`.
Right now, the delays are variable-rate but only at whole
numbers, which is kind of awkward. However, they can have
floating-point lengths. Here are their starting lengths and
rates, as well as their inputs which are set in `Synth::sample()`:

| delay | length (s) | rate   | input                            |
| ----- | ---------- | ----   | -------------------------------- |
| 1     | 6          | 1      | (0.5raw + 0.25del2 + 0.25del3)/3 |
| 2     | 0.05       | 2 or 5 | raw                              |
| 3     | 0.025      | 3      | del2                             |

Delay 2 switches to rate 5 whenever the incoming sample is <
0.05. I realize I didn't take its absolute value, so that
basically means "whenever it's negative ar close to." I tried
doing it based on the absolute value of the sample and it
definitely sounds less cool, so um, way to go me for not
remembering to do that. :P My original idea was that it would add
in a bit of extra high-pitched-ness whenever the music got quiet
in some sense, but I guess in practice it's more like the delay
rapidly changes between rates 2 and 5 all the time which creates
a bunch of cool-sounding hissing. Yeah! :D

After that, the raw sample and delay output are mixed together
and scaled and then split into left and right channels. The left
and right channels are scaled with more LFOs at slightly
different rates, then mixed-cross channel somewhat and scaled one
last time. Then we clip both samples at -1.0 and 1.0 so that we
don't end up with out-of-range samples (something more
sophisticated will ultimately be needed there probably), and
we're basically done! I could go into more detail about this
section but it's basically just a lot of adding and scaling
signals which I think I've already pretty well established I need
some kind of general-purpose representation of.

## Interface?

So um, already we have it so that you can pass curve descriptions
into `Synth()` sort of, if in a very special-cased way. Naturally
you could also pass the descriptions in for the envelope and LFO
curves. (This is giving me the feeling that the wrap modes should
be part of the DSL too.) There is still the question of how
exactly `Synth` should instatiate, store, and communicate with
them, and how and what we should request that `Synth` do with
them.

In `sketching_the_audio_vm.md`, I was taking the perspective that
a `Curve` would live within a predefined `Param`, specifically
within one of its curve slots. Now I feel like that was perhaps a
bit naïve, seeing as how I treated the `Curve`s in this piece
more like abstract signal generators I could tap at varying rates
and send wherever, mix together, etc. At the very least, I think
with the params/curve slots approach, I would want to be able to
"plug in" the same "curve signal" in multiple different slots in
different places and things, implying that the curves should have
more of an independent life à la oscillator modules or signal
generators or what have you.

One thing that might be worth a note is my inclination to use the
`env_pos` variable to reset or "sync" the envelopes periodically.
It might not be unreasonable to expose that as a kind of "binary
port" that you could feed a pulse train into, like where it "goes
off" whenever it changes sign (I'm sure there's some kind of
technical electronics term for this ;^^).

I'm starting to feel like, instead of `Synth` just holding
generic "events" or whatnot, it should hold:

* signal generators, which hold a curve and have rate and sync
  "ports"
* a mixer or mixers, where you can combine and scale signals
* a sequencer, which can output pitch, arbitrary normalized
  -1.0–1.0 "voltage", integer/stepped signals, sync, etc., and
  can be programmed with the tracker interface (and maybe in other
  ways too, like it has its own signal inputs? maybe like, the
  tempo/time signature/swing controls and things?)
* effects, which take an audio signal, return an audio signal,
  and also have various "control ports"

This is even more like Csound than before. XD I do ultimately
think it's more natural in this kind of software environment to
think more in terms of signals or "voltages" than in terms of
pots and keys and buttons and things like I have been. Not that
those are terrible metaphors or anything, I'm just starting to
feel like it's good to place them kind of judiciously in the
conceptual space or w/e. It's good to remember that this is kind
of a "robot's environment" that isn't very tactile or what have
you. :P

I'm sort of feeling like `Synth` can play the role of the clock,
by pinging everything when a sample request comes in, and that it
can then hand out the sum of whatever has been marked as output
or whatnot. It's already sort of playing that role now, with
`pos`.

On that note, I think I should "redo time" XD, more along the
lines of what I did with the FM synth (which will be a boon to
bringing it into this program, too :D).

Of course, with this specific piece, the atmosphere is less that
of a sequencer and more that of a box that can switch between two
sets of pitches driving a wavetable synth that can switch between
two different waveforms, with both of them being controlled by an
"LFO mixed with a DC offset" or what have you (`ramp_time`).

Hmmm, I think that seems like it might be a fruitful way of
thinking. Maybe I should try to rephrase the program in more
signal-flavored terms.

## Signlaz

So, the `soidser` wavetables, as well as the `segs` envelopes (I
really ought to change that to `seg`, it only does one :P) and
the `soid` LFO, would all be used to configure signal generators,
which would live in their own special part of the program. There
would be a sequencer with connections to the wavetable generators
controlling their output rates. The sequencer itself could expose
a port that manually controls its "read head," and that could be
driven by…whatever `high_chd` turns into.

I guess, if we sort of arbitrarily say that the chords are at
positions 1 and 2 in the sequencer's score, `high_chd` would be
something that you start, then it continues until it passes
`ramp_time_2` elapsed time, then toggles between 1 and 2. Sounds
like a variable-length delay carrying an impulse! And something
that switches its output between one signal and another whenever
it gets pinged (transistor? :P). You could have 1 and 2 as its
two inputs and plug the `ramp_time_2` delay into its toggle.
`ramp_time_2`, of course, could be an amplified signal from the
LFO mixed with a "DC voltage" of 1/3 like I put it before,
controlling the delay's length. I think that would take care of
the pitch and timbre! Although I still need to determine what
kicks off `high_chd`.

As a side note, my thinking is that a sequencer would have a
clock port that would advance it based on its tempo and swing
controls and such, but you could also opt not to connect this and
instead to just control its read head manually. I'm thinking you
could even have one sequencer control another in this manner,
which I might do to add more onto this piece when I get to that
point.

Anyway, what about the envelopes? Those get restarted every time
`in_cs2` goes off. `in_cs2` switches between "on" and "off" at
intervals of three seconds off, one second on. (It's what kicks
off `high_chd`, as a side note, both on the up- and the
downstroke.) I guess I could accomplish this with a delay that
carries an impulse and starts with a 3-second period, but then
when the impulse comes out, it triggers a switch that changes the
delay's period to 1-second, then when that impulse comes out, it
triggers the switch again and it turns the period back to 3
seconds.

As another side note, I'm thinking it would probably be good to
have a "pulse train" module or w/e that just emits a pulse at
some variable rate. That would save the huge amount of needless
memory consumption that using long delays just to carry single
impulses would entail.

The different volumes of the "main" and "supplementary"/"color"
tones in the high chord could be achieved using a mixer following
the wavetable outputs. This implies per-track outputs from the
sequencer in some sense, of course, which I guess I was basically
expecting.

The `delay_2.adjrate()` switching when `out < 0.05` implies that
there should be switches with a high/low setting that take a
continuous "voltage" as opposed to just switching from an
impulse.

Other than that, I think the stuff with the delays and stereo
splitting and such can be handled based on things I've already
discussed. Yay!! I think this might actually work and be really
cool! I definitely have plenty more coding to do now. :P
