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

    Curves(std::filesystem::path db_path)
        : curves {std::make_shared<CurveDB>(db_path)}
    {}

    Curve newc(std::string name)
    {
        return {curves, name};
    }

    Curve get(DB::id_t id)
    {
        return curves->get(id);
    }

private:
    curves_col curves;
};

#endif
