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

    class CurveAlg {
    public:
        virtual entry_t inner_process(Curve&, seek_t pos) = 0;

        virtual ~CurveAlg() = default;

        void process(Curve&, double endpos = 1.0, double startpos = 0.0);
    };

    class Segs : public CurveAlg {
    public:
        Segs(double speed = 0.0, double startval = 0.0, double endval = 1.0);

        entry_t inner_process(Curve&, seek_t pos) override;

    private:
        double speed;
        double startval;
        double endval;
    };

    class Soid : public CurveAlg {
    public:
        Soid(double harmon   = 1.0,
             double phase    = 0.0,
             double strength = 1.0,
             double offset   = 0.0);

        entry_t inner_process(Curve&, seek_t pos) override;

    private:
        double harmon;
        double phase;
        double strength;
        double offset;
    };

    class Soidser : public CurveAlg {
    public:
        static constexpr double nsqrd_coefs  = -1.0;
        static constexpr double same_as_part = -2.0;

        Soidser(double part_spacing  = 1.0,
                double coef_spacing  = same_as_part,
                double phase_spacing = 0.0,
                double fund          = 1.0);

        entry_t inner_process(Curve&, seek_t pos) override;

    private:
        double part_spacing;
        double coef_spacing;
        double phase_spacing;
        double fund;
    };

    Curve(std::shared_ptr<CurveDB> ndb);

    Curve(std::shared_ptr<CurveDB> ndb, std::string nname);

    Curve(std::shared_ptr<CurveDB> ndb, DB::id_t nid);

    Curve(std::shared_ptr<CurveDB> ndb, DB::id_t nid, std::string nname);

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
    Curve& transeg(double speed    = 0.0,
                   double startval = 0.0,
                   double endval   = 1.0);

    Curve& sine(unsigned long sawl = 20);

    /* be careful using this pointer
     *
     * it's intended to make things easy working with C APIs, but obviously it
     * will only be valid for as long as the Curve is alive
     *
     * you are responsible for ensuring that you don't use this pointer after
     * the Curve is gone
     */
    const double* data() const { return table.data(); }

    Curve& save();

    entry_t get(seek_t read_head) const;

public:
    std::string name = "";

    DB::id_t id = DB::no_id;

    std::array<entry_t, tab_len> table = {};

    bool in_db = false;

private:
    std::shared_ptr<CurveDB> db;
};

#endif
