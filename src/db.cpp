#include "db.hpp"

#include <unistd.h>

DB::DB(std::filesystem::path dbfile)
{
    check_err<open_db_error>(sqlite3_open(dbfile.c_str(), &db_p), dbfile);

    prep_stmt("PRAGMA page_size = "
              + std::to_string(sysconf(_SC_PAGE_SIZE))
              + ";")
        .step();
}

DB::~DB() noexcept
{
    // "finalize all prepared statements, close all BLOB handles, and
    // finish all sqlite3_backup objects associated with the sqlite3
    // object"

    sqlite3_close(db_p);
}

DB::Statement DB::prep_stmt(stmt_src src)
{
    // 1. Create the prepared statement object using sqlite3_prepare_v2().
    // 2. Bind values to parameters using the sqlite3_bind_*() interfaces.
    // 3. Run the SQL by calling sqlite3_step() one or more times.
    // 4. Reset the prepared statement using sqlite3_reset() then go back to
    //    step 2. Do this zero or more times.
    // 5. Destroy the object using sqlite3_finalize(). 

    stmt* stmtp;
    const char* z_tail;

    check_err<prep_stmt_error>(sqlite3_prepare_v2(db_p,
                                                  src.c_str(),
                                                  src.length() + 1,
                                                  &stmtp,
                                                  &z_tail),
                               src); 

    stmts_p->emplace(src, stmtp);

    return {stmts_p, src};
}

//DB::stmt_handle_t DB::next_stmt_handle()
//{
//    stmt_handle_t limit = next_stmt_h - 1;
//    for (; next_stmt_h != limit; ++next_stmt_h) {
//        if (stmts_p->count(next_stmt_h) == 0) {
//            return next_stmt_h;
//        }
//    }
//
//    throw runtime_error("no sql statement handles remain");
//}

//void DB::step_stmt(stmt_handle_t h)
//{
//    check_err<step_stmt_error>(sqlite3_step(stmts.at(h).p),
//                               stmts.at(h).source());
//}

void DB::delete_stmt::operator()(sqlite3_stmt* p)
{
    // this returns an error code if the most recent evaluation of the statement
    // failed, but i'm not sure that's something we really need to worry about
    // in its destructor
    sqlite3_finalize(p);
}

//bool DB::has_data(stmt_handle_t h)
//{
//    return (sqlite3_data_count(stmts.at(h).p) != 0);
//}

//void DB::bind(stmt_handle_t, int sql_param_ndx, const void* blob_data, int sz)
//{
//    // sqlite3_bind_blob(sqlite3_stmt*, int, const void*, int n, void(*)(void*));
//    // sqlite3_bind_blob(sqlite3_stmt*, sql_param_ndx, blob_data, sz, SQLITE_TRANSIENT);
//}

