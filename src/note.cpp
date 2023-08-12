#include "note.hpp"

#include <cstdint>
#include <climits>
#include <string>
#include <sstream>
#include <cstdio>

Note::Note(octave_t octave, pc_t pitchc, frac_t frac)
    : oct {octave}, pc {pitchc}, fr {frac}, hz {edo_pitch(octave, pitchc, frac)}
{}

Note::Note(enum Note::status)
    : oct {0}, pc {0}, fr {0.0}, hz {0.0}
{}

bool Note::is_off() const
{
    return hz == 0.0;
}

const std::array<Note::freq_t, Note::steps_cnt> Note::edo_pitches = []
{
    std::array<freq_t, steps_cnt> ep {};

#ifdef NDEBUG
    std::printf("All Note freqs:\n");
#endif

    for (size_t i = 0; i < steps_cnt; ++i) {
        octave_t o = i / edo;
        pc_t     p = i % edo;

        ep[i] = base_pitch * std::pow(2.0, o + static_cast<double>(p)
                                               / static_cast<double>(edo));

#ifdef NDEBUG
        std::printf("%0.14f\n", ep[i]);
#endif
    }

#ifdef NDEBUG
    std::printf("\n");
#endif

    return ep;
}();

Note::freq_t Note::edo_pitch(octave_t o, pc_t p)
{
    return edo_pitches.at(o*edo + p);
}

Note::freq_t Note::edo_pitch(octave_t o, pc_t p, frac_t f)
{
    if (o == 0) { o = 1; }
    size_t ndx = (o - 1)*edo + p;
    if (ndx >= steps_cnt - 1) {
        return edo_pitches.at(steps_cnt - 1);
    } else {
        auto floor = edo_pitches.at(ndx);
        auto ceil  = edo_pitches.at(ndx + 1);
        return floor + f*(ceil - floor);
    }
}

std::string Note::pch() const
{
    std::array<char, 18> fs;
    std::snprintf(fs.data(), fs.size(), "%.15g", fr);
    std::stringstream s;
    s << oct << "." << pc << std::string_view(fs.data() + 2, fs.size() - 2);
    return s.str();
}

Note Note::operator+(frac_t npcf) const
{
    signed_pc_t npc = std::floor(npcf);
    frac_t      nf;
    if (npcf >= 0.0) {
        nf = npcf - std::floor(npcf);
    } else {
        nf = npcf - std::ceil(npcf);
    }

    signed_pc_t spc = (static_cast<signed_pc_t>(pc) + npc) % edo;
    signed_pc_t soct = static_cast<signed_pc_t>(oct)
                       + ((static_cast<signed_pc_t>(pc) + npc)
                          / static_cast<signed_pc_t>(edo));
    frac_t      snf = std::fmod((fr + nf), 1.0);

    if (soct > octave_max
        || (soct == octave_max && (spc > 0 || snf > 0.0))) {
        soct = octave_max;
        spc  = 0;
        snf  = 0.0;
    } else if (soct < 1) {
        soct = 1;
        spc  = 0;
        snf  = 0.0;
    }

    return {
        static_cast<octave_t>(soct),
        static_cast<pc_t>(spc),
        snf,
    };
}

Note Note::operator+(const Note& nn) const
{
    return *this + (nn.octave()*edo + nn.pitchc() + nn.frac());
}

Note Note::operator-(frac_t npcf) const
{
    return *this + (-npcf);
}

Note Note::operator-(const Note& nn) const
{
    return *this + (-(nn.octave()*edo + nn.pitchc() + nn.frac()));
}

Note Note::floor() const
{
    return Note(octave(), pitchc());
}

Note Note::ceil() const
{
    if (octave() == octave_max) {
        return *this;
    } else {
        return *this + Note(0, 1);
    }
}
