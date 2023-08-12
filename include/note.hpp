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
    typedef int_fast32_t  signed_pc_t;
    typedef double        frac_t;
    typedef double        freq_t;
    typedef freq_t        sig_t;

    static constexpr pc_t   edo        = 53;
    static constexpr freq_t base_pitch = 20.0;

    static constexpr octave_t octave_max = 10; // 20480 Hz
    static constexpr size_t   steps_cnt  = octave_max*edo;
    static constexpr frac_t   frac_max   = UINT_FAST32_MAX;

    static const std::array<freq_t, steps_cnt> edo_pitches;

    static freq_t edo_pitch(octave_t, pc_t);
    static freq_t edo_pitch(octave_t, pc_t, frac_t);

    enum status {
        off
    };

    Note() = default;
    Note(octave_t, pc_t, frac_t f = 0);
    Note(enum status);

    octave_t octave() const { return oct; }
    pc_t     pitchc() const { return pc; }
    frac_t   frac()   const { return fr; }
    freq_t   freq()   const { return hz; }
    sig_t    get()    const { return freq(); }

    std::string pch() const;

    Note operator+(frac_t) const;
    Note operator+(const Note&) const;
    Note operator-(frac_t) const;
    Note operator-(const Note&) const;

    ~Note() = default;

    bool is_off() const;

    Note floor() const;
    Note ceil() const;

private:
    octave_t oct;
    pc_t     pc;
    frac_t   fr;
    freq_t   hz;
};

#endif
