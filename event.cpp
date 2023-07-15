#include "event.hpp"

#include <cstdint>
#include <climits>
#include <string>
#include <sstream>

std::size_t Event::width() const
{
    return str().length();
}

Note::Note(octave_t octave, pc_t pitchc, frac_t frac)
    : oct {octave}, pc {pitchc}, fr {frac}
{
    
}

NoteView::NoteView(std::string octave, std::string pcfrac)
{
     auto oct =
         static_cast<Note::octave_t>(std::stoul(octave) & Note::octave_max);

     auto pc = static_cast<Note::pc_t>(std::stoul(pcfrac.substr(0, pc_digits)));

     auto frac_raw = std::stoul(pcfrac.substr(pc_digits,
                                              pcfrac.length() - pc_digits));
     auto frac = static_cast<Note::frac_t>(frac_raw & Note::frac_max);

     nt = {oct, pc, frac};
}

std::string NoteView::str() const
{
    std::stringstream s;
    s << nt.octave() << "." << nt.pitchc() << nt.frac();
    return s.str();
}
