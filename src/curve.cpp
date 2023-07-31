#include "curve.hpp"
#include "curve_db.hpp"

#include <cmath>

Curve::Curve(std::shared_ptr<CurveDB> ndb)
    : db {ndb}
{}

Curve::Curve(std::shared_ptr<CurveDB> ndb, std::string nname)
    : db {ndb},
      name {nname}
{}

Curve::Curve(std::shared_ptr<CurveDB> ndb, DB::id_t nid)
    : db {ndb},
      id {nid}
{}

Curve::Curve(std::shared_ptr<CurveDB> ndb, DB::id_t nid, std::string nname)
    : db {ndb},
      id {nid},
      name {nname}
{}

Curve& Curve::transeg(double speed,
                      double startval,
                      double endval)
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

Curve& Curve::sine()
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

Curve& Curve::save()
{
    db->emplace(*this, id);
    return *this;
}

Curve::entry_t Curve::get(seek_t read_head) const
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
