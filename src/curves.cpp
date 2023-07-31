#include <curves.hpp>

Curves::Curves(std::filesystem::path db_path)
    : curves {std::make_shared<CurveDB>(db_path)}
{}

Curve Curves::newc(std::string name)
{
    return {curves, name};
}

Curve Curves::get(DB::id_t id)
{
    Curve c {curves};
    curves->get(c, id);
    return c;
}
