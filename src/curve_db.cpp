#include "curve_db.hpp"
#include "curve.hpp"

#include <cstring>

const std::string CurveDB::table_name = "curves";
const std::string CurveDB::id_col     = "id";
const std::string CurveDB::name_col   = "name";
const std::string CurveDB::vals_col   = "vals";

void CurveDB::emplace(Curve& c, int ndx)
{
    auto curve_sz = std::remove_reference<decltype(c)>::type::sz_bytes;

    std::string src = "INSERT INTO " + table_name
                      + " (" + name_col + "," + vals_col + ") VALUES (?,?";
    if (ndx != DB::no_id) {
        src += ",?";
    }
    src += ");";

    auto stmt = prep_stmt(src);

    if (ndx != DB::no_id) {
        stmt.bind(ndx);
    }

    stmt.bind(c.name)
        .bind(c.data(), curve_sz)
        .step();
}

Curve CurveDB::get(id_t id)
{
    Curve c;
    const double* vals_p;

    auto select_c =
        prep_stmt("SELECT " + id_col + "," + name_col + "," + vals_col +
                  " from " + table_name + ";")
            .step()
            .col(0, c.id)
            .col(1, c.name)
            .col(2, vals_p);

    std::memcpy(c.table.data(), vals_p, select_c.col_sz(2));

    return c;
}