#ifndef b3bdd72cefb34aed94103c3411dfea65
#define b3bdd72cefb34aed94103c3411dfea65

#include <cstdint>
#include <string>

class Event {
public:
    virtual std::string str() const = 0;
    virtual std::size_t width() const;

    virtual ~Event() = default;
private:
};

class Note {
public:
    typedef uint_fast16_t octave_t;
    typedef uint_fast16_t pc_t;
    typedef uint_fast32_t frac_t;

    static constexpr octave_t octave_max = UINT_FAST16_MAX;
    static constexpr pc_t     pc_max     = UINT_FAST16_MAX;
    static constexpr frac_t   frac_max   = UINT_FAST32_MAX;

    static constexpr pc_t edo = 53;

    Note() = default;
    Note(octave_t, pc_t, frac_t);

    octave_t octave() const { return oct; }
    pc_t     pitchc() const { return pc; }
    frac_t   frac()   const { return fr; }

    ~Note() = default;

private:
    octave_t oct;
    pc_t     pc;
    frac_t   fr;
};

class NoteView {
public:
    NoteView(std::string octave, std::string pcfrac);

    std::string str() const;

private:
    static constexpr Note::pc_t pc_digits = []{
        Note::pc_t n = Note::edo;
        Note::pc_t cnt = 0;

        do {
            n /= 10;
            ++cnt;
        } while (n != 0);

        return cnt;
    }();

    Note nt;
};

#endif
