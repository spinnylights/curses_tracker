#ifndef bc626a914d6747eda66c0106cef7e39d
#define bc626a914d6747eda66c0106cef7e39d

#include <cstdint>
#include <cmath>
#include <string>
#include <array>
#include <sstream>

class Note {
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
    Note(octave_t, pc_t, frac_t);

    octave_t octave() const { return oct; }
    pc_t     pitchc() const { return pc; }
    frac_t   frac()   const { return fr; }
    freq_t   freq()   const { return hz; }

    std::string pch() const;

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
