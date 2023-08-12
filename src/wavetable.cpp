#include "wavetable.hpp"

#include <fstream>

Wavetable::Wavetable(std::string ncurve_desc)
    : curve_desc {ncurve_desc}
{}

void Wavetable::update(ticks npos)
{
    pos = npos;
}

void Wavetable::note(Note n)
{
    cur_note = n;

    if (cur_note.is_off()) {
        cur_note = Note::off;
    } else {
        std::string desc = curve_desc + " " + std::to_string(cur_note.freq());

        cs.try_emplace(cur_note, desc);
        if (n.octave() < Note::octave_max) {
            cs.try_emplace(cur_note + Note(0, 1), desc);
        }
    }
}

Curve::entry_t Wavetable::get() const
{
    if (cur_note.is_off()) {
        return 0.0;
    } else if (cur_note.frac() == 0.0) {
        return cs.at(cur_note).get(time_f(pos).count()*cur_note.freq());
    } else {
        auto lower = cs.at(cur_note.floor()).get(time_f(pos).count());
        auto upper = cs.at(cur_note.ceil()).get(time_f(pos).count());

        return lower + (upper - lower)*(cur_note.frac());
    }
}
