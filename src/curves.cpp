#include <curves.hpp>

Curves::Curves(std::filesystem::path db_path)
    : curves {std::make_shared<CurveDB>(db_path)}
{}

Curve Curves::newc(std::string name)
{
    Curve::args a {};
    a.db = curves;
    a.name = name;
    return {a};
}

Curve Curves::newc()
{
    Curve::args a {};
    a.db = curves;
    return {a};
}

Curve Curves::get(DB::id_t id)
{
    Curve::args a {};
    a.db = curves;
    Curve c {a};
    curves->get(c, id);
    return c;
}
