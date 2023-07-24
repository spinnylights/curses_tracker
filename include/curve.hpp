#ifndef m888aa58dbc740dab62b1222e3e1ec21
#define m888aa58dbc740dab62b1222e3e1ec21

/* this can either be done as a table that you lerp/cerp through or as a
 * function you run each time; i'll have to think about which makes the most
 * sense and how
 *
 * each curve in the slot can be multiplicative (scaling) or additive (offset)
 * by default the curve is as long as the note, but it can be shorter or longer
 *     (in proportion to the note length or fixed), repeat with its own "repeat
 *     param", etc.(?)
 * when a curve finishes, the last value of the curve is folded into the fixed
 *     scalar and the slot's value is reset to 0; in this sense, curves are
 *     "sticky"
 *
 * probably some of that should be in a "curve slot" class instead of the curve
 * itself
 *
 * ways to describe a curve
 *     - transeg/gen16
 *         each segment:
 *             - start
 *             - curve
 *             - end
 *             - length
 *     - cubic polynomials (gen05)
 *         each segment:
 *             - start
 *             - inflection
 *             - end
 *             - length
 *     - sinusoids (gen19)
 *         - fractional partial number
 *         - relative strength of each partial
 *         - initial phase
 *         - dc offset
 *     - straight lines (gen27)
 *         each point (lines between each):
 *             - x
 *             - y
 *
 * curves are normalized 0–1 on x and -1–1(always?) on y
 *
 * sine      |
 * triangle *|
 * square    |
 * saw       |
 */

#include "curve_db.hpp"

#include <cstdint>
#include <array>
#include <vector>
#include <memory>
#include <string>
#include <cmath>
#include <algorithm>

class Curve {
public:
    typedef double  entry_t;
    typedef double  seek_t;

    static constexpr std::size_t tab_len  = 16384;
    static constexpr seek_t      tab_lenf = static_cast<double>(tab_len);
    /* this is an int because of sqlite, not because it's necessarily sensible
     * to store this way
     *
     * fortunately you probably have a compiler available that can warn if this
     * wouldn't fit (e.g. Clang's `-Wconstant-conversion`)
     */
    static constexpr int         sz_bytes = tab_len * sizeof(entry_t);

    Curve(std::shared_ptr<CurveDB> ndb, std::string nname = "")
        : db {ndb},
          name {nname}
    {}

    Curve(std::shared_ptr<CurveDB> ndb, DB::id_t nid, std::string nname = "")
        : db {ndb},
          id {nid},
          name {nname}
    {}

    Curve() = default;

    /* by default, the transeg curve goes from 0.0 to 1.0
     * a speed of 0.0 is linear
     * >0.0 means it approches the end more quickly(?)
     * <0.0 means it approches the end more slowly(?)
     *
     * transeg()               -- starts at 0.0, goes linearly to 1.0
     * transeg(3.0)            -- starts at 0.0, goes quickly to 1.0
     * transeg(3.0, -1.0)      -- starts at -1.0, goes quickly to 1.0
     * transeg(3.0, -1.0, 0.0) -- starts at -1.0, goes quickly to 0.0
     */
    auto transeg(double speed    = 0.0,
                 double startval = 0.0,
                 double endval   = 1.0)
    {
        if (speed == 0.0) {
            for (std::size_t i = 0; i < tab_len; ++i) {
                table[i] = startval + (i/tab_lenf)*(endval - startval);
            }
        } else {
            for (std::size_t i = 0; i < tab_len; ++i) {
                double numer = (endval - startval)
                               * (1.0 - std::exp(i*speed / (tab_len - 1)));
                double denom = 1.0 - std::exp(speed);
                table[i] = startval + numer / denom;
            }
        }
        return *this;
    }

    auto sine()
    {
        for (std::size_t i = 0; i < tab_len; ++i) {
            auto basic_freq = 2*M_PI*(i/tab_lenf);
            decltype(basic_freq) saw = 0;
            static constexpr short sawl = 20;
            for (short j = 1; j < sawl; ++j) {
                saw += (std::cos(basic_freq*j)/j);
            }
            table[i] = saw;
        }

//        for (auto& v : table) {
//            bool neg = std::signbit(v);
//            v = std::log(std::abs(v)) + 1;
////            if (std::signbit(v)) {
//            if (neg) {
//                v *= -1;
//            }
//        }

        auto max_mag =
            std::abs(*std::max_element(table.begin(), table.end(), [](int a, int b)
            {
                return std::abs(a) < std::abs(b);
            }));

        if (max_mag > 1.0) {
            for (auto& v : table) {
                v /= max_mag;
            }
        }

        return *this;
    }

    /* be careful using this pointer
     *
     * it's intended to make things easy working with C APIs, but obviously it
     * will only be valid for as long as the Curve is alive
     *
     * you are responsible for ensuring that you don't use this pointer after
     * the Curve is gone
     */
    const double* data() const { return table.data(); }

    auto save()
    {
        db->emplace(*this, id);
        return *this;
    }

    entry_t get(seek_t read_head) const
    {
        if (read_head > 1.0) { read_head = 1.0; }
        if (read_head < 0.0) { read_head = 0.0; }

        seek_t tab_dist = read_head * (tab_lenf - 1.0); 
        auto left_ndx = static_cast<decltype(tab_lenf)>(std::floor(tab_dist));
        entry_t left  = table.at(left_ndx);
        entry_t right =
            table.at(static_cast<decltype(tab_lenf)>(std::ceil(tab_dist)));
        seek_t rel_dist = tab_dist - left_ndx; 
        return left + rel_dist*(right - left);
    }

public:
    std::string name = "";

    DB::id_t id = DB::no_id;

    std::array<entry_t, tab_len> table = {};

private:

    std::shared_ptr<CurveDB> db;
};

#endif
