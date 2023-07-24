#include "curve.hpp"
#include "curve_db.hpp"

#include <cmath>

//void Curve::transeg(double speed,
//                    double startval,
//                    double endval)
//{
//    for (std::size_t i = 0; i < tab_len; ++i) {
//        double numer = (endval - startval)
//                       * (1.0 - std::exp(i*speed / (tab_len - 1)));
//        double denom = 1.0 - std::exp(speed);
//        table[i] = startval + numer / denom;
//    }
//}

//auto Curve::save()
//{
//    db->emplace(*this, 2);
//    return *this;
//}
