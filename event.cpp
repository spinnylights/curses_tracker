#include "event.hpp"

#include <cstdint>
#include <climits>
#include <string>
#include <sstream>

Note::Note(pos_t p, octave_t octave, pc_t pitchc, frac_t frac)
    : oct {octave}, pc {pitchc}, fr {frac}
{
    position = p;
}

const std::array<Note::freq_t, Note::steps_cnt> Note::edo_pitches = []{
    std::array<freq_t, steps_cnt> ep {};
    for (size_t i = 0; i < steps_cnt; ++i) {
        octave_t o = i / edo;
        pc_t     p = i % edo;

        ep[i] = base_pitch * std::pow(2.0, o + static_cast<double>(p)
                                               / static_cast<double>(edo));
    }
    return ep;
}();

Note::freq_t Note::edo_pitch(octave_t o, pc_t p)
{
    return edo_pitches.at(o*edo + p);
}

Note::freq_t Note::edo_pitch(octave_t o, pc_t p, frac_t f)
{
    size_t ndx = o*edo + p;
    if (ndx >= steps_cnt - 1) {
        return edo_pitches.at(steps_cnt - 1);
    } else {
        auto floor = edo_pitches.at(ndx);
        auto ceil  = edo_pitches.at(ndx + 1);
        return floor + f*(ceil - floor);
    }
}

//NoteView::NoteView(std::string octave, std::string pcfrac)
//{
//     auto oct =
//         static_cast<Note::octave_t>(std::stoul(octave) & Note::octave_max);
//
//     auto pc = static_cast<Note::pc_t>(std::stoul(pcfrac.substr(0, pc_digits)));
//
//     auto frac_raw = std::stoul(pcfrac.substr(pc_digits,
//                                              pcfrac.length() - pc_digits));
//     auto frac = static_cast<Note::frac_t>(frac_raw & Note::frac_max);
//
//     nt = {oct, pc, frac};
//}
//
//std::string NoteView::str() const
//{
//    std::stringstream s;
//    s << nt.octave() << "." << nt.pitchc() << nt.frac();
//    return s.str();
//}
