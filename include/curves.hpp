#ifndef Ef4d028b0bed4de392283bb37868a076
#define Ef4d028b0bed4de392283bb37868a076

#include "curve.hpp"
#include "curve_db.hpp"

#include <memory>
#include <vector>
#include <filesystem>

class Curves {
public:
    typedef std::shared_ptr<CurveDB> curves_col;

    Curves(std::filesystem::path db_path, double sample_rate);

    Curve newc(std::string name);
    Curve newc();

    Curve get(DB::id_t id);

private:
    curves_col curves;
    double sample_rate;
};

#endif
