# Audio VM sketch

Here I am thinking about a sort of "audio synthesis virtual
machine" which can be embedded in arbitrary programs and receives
streams of operations. I think it would be neat if there was a
sort of "stream template" type that exposed various you could
adjust to generate different streams along a certain line in some
sense. It's possible that this type could provide support for
instruments or effects even within this program—I'll have to see.

Anyway, the way I've been picturing a track is as a kind of
sparse array of synth events indexed by (time) tick. In other
words, a track is a single musical line; if you wanted to play
chords, you would use three different tracks. This is like,
"tracker style" tracks.

Also, following the "tracker style" mold, each of the tracks has
an associated instrument, set of effect outputs, and a set of
parameters formed from both the instrument and effects as well as
standard all-track parameters like amplitude. I have the idea
that a track will support "shadow tracks" that copy it in every
respect except for procedural differences the user specifies
(e.g. a shadow track with "offset 1/4 note, amp -0.2" would
create a quarter-note length delay 0.2 quieter than its source
track, a shadow track with "note +0.31" and another with "note
+1.00" would turn single note events into power chords, etc.).

(I just realized the shadow tracks will work best if they're
arranged as a graph, maybe a DAG—like, if you wanted to use both
of those at once, you might want the two "power chord" shadow
tracks to also receive the delay treatment.)

Anyway, when you're actually playing back the piece, I guess one
possibility is that you advance in time 1 tick, then check each
track (including shadows? or maybe you store the shadows on the
original track itself and duplicate the events accordingly? idk)
and if you find an event at that tick, you add it to the list of
current events. Each sample, you increment each of the current
events by the number of elapsed ticks, get audio samples from
them and sum them for the output sample (maybe run them through
effects? or should that happen within the track or instrument?
idk), and remove any events that report that they're done.

It would be possible to do all of this within the VM, but it's
not the only conceivable approach. Another option would be to
have some of this done by a sort of "track host" that takes a set
of track objects and converts them into a stream of VM commands,
which it passes to the VM. With that model, the VM itself might
not have the same picture of time that the track host does;
rather, it might just exist in a world of "current events" that
it's adding as it receives them and removing as they finish and
producing sound in-between.

I think to some extent this latter possibility might be more
graceful, because it would mean you could have the VM just kind
of hanging out and do things like press buttons on the keyboard
or play a MIDI instrument or something and have it emit audio in
real-time, as well as queue up a sequence of events to emit to
it gradually over time. I think, from this perspective, the VM
should live on its own thread and have a deque perhaps of current
events which it will empty every time it's signaled to somehow
(probably with the elapsed time). Its view of time might be
floating-point (maybe). There can be a separate stream player or
whatever that knows about ticks and can run through a set of
stored tracks and emit their events to the VM at the right
moment. Probably it will be signaled with a number of ticks to
advance by. It's possible they should both live on separate
threads.

Okay. So, with that settled, the VM is just taking events that
start at time 0 (whatever time representation we decide to use).
It doesn't know anything about tracks; it just advances each of
its events by some amount of time and sums up their audio and
removes events as they report that they're done. So, presumably
in that case it accepts commands that initiate events, and a
command that advances the time, and it produces audio samples
that have to be collected somehow by the SDL audio callback
thread (or w/e else is consuming them). It's probably save to
picture the VM as akin to a keyboard-based synth operated by a
million-armed octopus or that sort of thing.

Within the VM live a number of synthesis methods. Each event will
belong to one synthesis method (this is following from the
tracker-style design discussed above). Each synthesis method
will expose a set of parameters, and an event specifies Curves
for each of those accompanied by the sort of arguments you would
give Csound's `table` opcode: at the least, a 0–1-valued Curve
scaled over time for the seek position and a wrap/nowrap/ping-
pong setting for the seek curve. (Csound also provides an offset
if you want to start seeking from the middle of the table, but
I'm not sure that's really necessary since you could just set up
the seek Curve that way if you wanted.) Maybe for the simple case
you could also just pass a rate to increment the seek position by
and wrap/nowrap/ping-pong, instead of having to pass a 0–1 line
each time (although to be fair probably there would already be a
stored curve like that you could just reference.)

That's an important point: I think the VM should also support the
full Curve DSL, along with an ID to identify the curve by (or
maybe it returns an ID for you after you send a "render curve"
command or w/e). Then you can just create the curves you need and
after that all you need to pass in the events is the IDs of the
param and seek curves and the index rate and wrap setting for the
seek curve. So, I guess each "param statement" might look
something like:

```cpp
struct param_curve_stmt {
    curve_id        param_controller;
    curve_id        seeker = ramp;
    seek_rate_t     seek_rate;
    enum seek_style wrap_setting = nowrap;
};
```

I think you should also be able to pass just a static value,
though, instead of this. Maybe the VM can communicate with them
through an abstract interface that just takes a time increment
and returns a value like many other things here (perhaps that
should be a standard abstract interface, yes...).

As for the effects, I think each effect can have, like, a vector
or something in the VM where different effect instances are
stored by ID. When you set up an event, you can supply a graph or
maybe just a list of indices (?) where it threads its output
through various effects by ID. Maybe like, you can also submit
separate commands to change the parameters on a certain effect,
in the same manner as instrument effects (i.e. providing static
settings or curves that loop or don't loop or w/e).

So, I guess it sounds like there are three major classes of
commands for the VM:

* Render curve
* Control effect
* Start event

I realize that for the wavetable I actually want to generate a
whole family of curves that the wavetable synth can pick from by
note. This is like, something special the wavetable synth needs,
so in theory I could initialize the wavetable with this and have
it store the curves internally, but i think it would be sort of
nice to have them globally available just 'cos (who knows what i
might want to do with them ultimately). so, maybe there should be
a special command for that which takes `soidser` arguments.

```
curve 1 segs 5 1 0
notecurves 1 0.84893 0.7777777 0.98989898989
event wavetable (curves) 1 (length) 1 s.*<static amp> (note) 4.31 (static amp) 0.8 (amp curve) 1 [seek rate: scaled to length]
```

Maybe like, the VM input language doesn't really need to support
these kinds of multiplications—they can conceivably be performed
by whatever is generating these. Like, the length which changes
with amp/velocity there (which could also change with pitch)
could be calculated by whatever was emitting these events, since
it would know what the settings were and stuff.

Anyway, I think at this point I have enough to start coding from.
