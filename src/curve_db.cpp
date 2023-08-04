#include "curve_db.hpp"
#include "curve.hpp"

#include <cstring>

const std::string table_name = "curves";
const std::string id_col     = "id";
const std::string name_col   = "name";
const std::string vals_col   = "vals";

// remember to store + retrieve sample_rate for each Curve

CurveDB::CurveDB(std::filesystem::path dbfile)
    : DB(dbfile)
{
    auto check_table = prep_stmt("SELECT name FROM sqlite_master "
                                 "WHERE type='table'"
                                 " AND name='"
                                 + table_name
                                 + "';")
                           .step();

    if (!check_table.has_data()) {
        prep_stmt("CREATE TABLE "
                  + table_name
                  + "("
                  + id_col + " INTEGER PRIMARY KEY NOT NULL,"
                  + name_col + " TEXT NOT NULL,"
                  + vals_col + " BLOB"
                  ");")
            .step();
    }
}

// idk about this DB::no_id thing, maybe would be better to just have an
// overload that doesn't take ndx
DB::id_t CurveDB::emplace(Curve& c, int ndx)
{
    auto curve_sz = std::remove_reference<decltype(c)>::type::sz_bytes;

    std::string src = "INSERT INTO " + table_name + " (";

    if (ndx != DB::no_id) {
        src += id_col + ",";
    }

    src += name_col + "," + vals_col + ") VALUES (?,?";

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

    return last_id();
}

void CurveDB::update(Curve& c)
{
    auto curve_sz = std::remove_reference<decltype(c)>::type::sz_bytes;

    std::string src = "UPDATE " + table_name
                      + " SET " + name_col + "='?'"
                      + " WHERE " + id_col +  "=?;";
    prep_stmt(src)
       .bind(c.name)
       .bind(c.id)
       .step();

    sqlite3_blob* blob_p;

    check_err<open_blob_error>(sqlite3_blob_open(db_p,
                                            "main",
                                            table_name.c_str(),
                                            vals_col.c_str(),
                                            c.id,
                                            -1,
                                            &blob_p));

    check_err<write_blob_error>(sqlite3_blob_write(blob_p,
                                                   c.data(),
                                                   curve_sz,
                                                   0));

    check_err<close_blob_error>(sqlite3_blob_close(blob_p));
}

void CurveDB::get(Curve& c, id_t id)
{
    auto curve_sz = std::remove_reference<decltype(c)>::type::sz_bytes;

    auto select_c =
        prep_stmt("SELECT " + name_col
                  + " from " + table_name
                  + " WHERE " + id_col + "=" + std::to_string(id) + ";")
            .step()
            .col(0, c.name);
    c.id = id;
    c.in_db = true;

    sqlite3_blob* blob_p;

    check_err<open_blob_error>(sqlite3_blob_open(db_p,
                                                "main",
                                                table_name.c_str(),
                                                vals_col.c_str(),
                                                id,
                                                -1,
                                                &blob_p));

    check_err<read_blob_error>(sqlite3_blob_read(blob_p,
                                                 c.table.data(),
                                                 curve_sz,
                                                 0));

    check_err<close_blob_error>(sqlite3_blob_close(blob_p));
}
