#ifndef k32b81dfe81b4451b57247f7c86d880c
#define k32b81dfe81b4451b57247f7c86d880c

#include "db.hpp"

#include <iostream>

class Curve;

class CurveDB : public DB {
public:

    CurveDB(std::filesystem::path dbfile);

    DB::id_t emplace(Curve& c, int ndx);

    void update(Curve& c);

    void get(Curve& c, id_t id);
};

#endif
