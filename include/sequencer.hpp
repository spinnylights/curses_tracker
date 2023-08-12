#ifndef I40ac6e2a7c84ee88a8c256ab7c91f3f
#define I40ac6e2a7c84ee88a8c256ab7c91f3f

#include "track.hpp"
#include "defs.hpp"
#include "note.hpp"

#include <vector>

class Sequencer {
public:
    typedef Track<Note>               note_track;
    typedef std::vector<note_track>   note_track_col;
    typedef note_track_col::size_type track_ndx;

    track_ndx add_track();
    void add(track_ndx, ticks, Note);

    void setpos(ticks);
    void update();

    Note get_note(track_ndx);

private:
    note_track_col note_trs;
    ticks pos {0};
};

#endif
