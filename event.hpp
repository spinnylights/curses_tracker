#ifndef b3bdd72cefb34aed94103c3411dfea65
#define b3bdd72cefb34aed94103c3411dfea65

#include <cstdint>
#include <cmath>
#include <string>
#include <array>

class Event {
public:
    typedef unsigned long long pos_t;

    virtual ~Event() = default;

    pos_t pos() const { return position; }

protected:
    pos_t position = 0;
};

class Note : public Event {
public:
    typedef uint_fast16_t octave_t;
    typedef uint_fast16_t pc_t;
    typedef double        frac_t;
    typedef double        freq_t;

    static constexpr pc_t   edo        = 53;
    static constexpr freq_t base_pitch = 1.02197503906;

    static constexpr octave_t octave_max = 14;
    static constexpr pc_t     pc_max     = edo - 1;
    static constexpr size_t   steps_cnt  = octave_max*edo + 17; // ~20913 Hz
    static constexpr frac_t   frac_max   = UINT_FAST32_MAX;

    static const std::array<freq_t, steps_cnt> edo_pitches;

    static freq_t edo_pitch(octave_t, pc_t);
    static freq_t edo_pitch(octave_t, pc_t, frac_t);

    Note() = default;
    Note(pos_t, octave_t, pc_t, frac_t);

    octave_t octave() const { return oct; }
    pc_t     pitchc() const { return pc; }
    frac_t   frac()   const { return fr; }
    freq_t   freq()   const { return hz; }

    ~Note() = default;

private:
    octave_t oct;
    pc_t     pc;
    frac_t   fr;
    freq_t   hz;
};

//class NoteView {
//public:
//    NoteView(std::string octave, std::string pcfrac);
//
//    std::string str() const;
//
//private:
//    static constexpr Note::pc_t pc_digits = []{
//        Note::pc_t n = Note::edo;
//        Note::pc_t cnt = 0;
//
//        do {
//            n /= 10;
//            ++cnt;
//        } while (n != 0);
//
//        return cnt;
//    }();
//
//    Note nt;
//};

#endif
