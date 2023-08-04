#include <curves.hpp>

Curves::Curves(std::filesystem::path db_path, double nsample_rate)
    : curves {std::make_shared<CurveDB>(db_path)},
      sample_rate {nsample_rate}
{}

Curve Curves::newc(std::string name)
{
    Curve::args a {};
    a.db = curves;
    a.name = name;
    a.sample_rate = sample_rate;
    return {a};
}

Curve Curves::newc()
{
    Curve::args a {};
    a.db = curves;
    a.sample_rate = sample_rate;
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
