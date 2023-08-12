#ifndef b8e1ec22fea5443abda403ba1bf05781
#define b8e1ec22fea5443abda403ba1bf05781

#include "curve.hpp"

#include <unordered_map>
#include <functional>

class Wavetable {
public:
    Wavetable(std::string curve_desc); // soidser, w/out freq (maybe should
                                       // take soidser args more directly)

    void note(Note);

    void update(ticks);

    Curve::entry_t get() const;

private:
    // of note, this assumes a static EDO
    struct NoteHasher {
        std::size_t operator()(Note const& n) const noexcept
        {
            return std::hash<Note::octave_t>()(n.octave())
                   ^ std::hash<Note::pc_t>()(n.pitchc());
        }
    };

    struct NoteEq {
        bool operator()(Note const& ln, Note const& rn) const noexcept
        {
            return (ln.octave() == rn.octave()) && (ln.pitchc() == rn.pitchc());
        }
    };

    std::unordered_map<Note, Curve, NoteHasher, NoteEq> cs;
    std::string curve_desc;
    Note cur_note;
    ticks pos {0};
};

#endif
