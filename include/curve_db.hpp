#ifndef k32b81dfe81b4451b57247f7c86d880c
#define k32b81dfe81b4451b57247f7c86d880c

#include "db.hpp"

#include <iostream>

class Curve;

class CurveDB : public DB {
public:
    static const std::string table_name;
    static const std::string id_col;
    static const std::string name_col;
    static const std::string vals_col;

    CurveDB(std::filesystem::path dbfile)
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

    void emplace(Curve& c, int ndx);

    Curve get(id_t id);
};

#endif
