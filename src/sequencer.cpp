#include "sequencer.hpp"

void Sequencer::add(track_ndx n, ticks t, Note note)
{
    note_trs.at(n).add(t, note);
}

void Sequencer::setpos(ticks newpos)
{
    pos = newpos;
    update();
}

void Sequencer::update()
{
    // ???
    for (auto& t : note_trs) {
        t.update(pos);
    }
}

Sequencer::track_ndx Sequencer::add_track()
{
    note_trs.emplace_back(note_track());

    return note_trs.size() - 1;
}

Note Sequencer::get_note(track_ndx n)
{
    return note_trs.at(n).get();
}
