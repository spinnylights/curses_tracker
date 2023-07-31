#include "db.hpp"

#include <unistd.h>

DB::Statement::Statement(stmts_col_p instmts_p, stmt_src insrc)
    : stmts_p {instmts_p},
      src {insrc}
{};

// i don't think this is async-safe atm
void DB::Statement::op(std::function<void(stmt*)> fn)
{
    auto p = stmts_p->at(src).release();
    fn(p);
    stmts_p->at(src).reset(p);
}

DB::Statement& DB::Statement::step()
{
    op([this](auto p) {
        check_err<step_stmt_error>(sqlite3_step(p), source());
    });
    bind_ndx = 1;
    return *this;
}

bool DB::Statement::has_data()
{
    bool out;
    op([&out](auto p) {
        out = (sqlite3_data_count(p) != 0);
    });
    return out;
}

DB::Statement& DB::Statement::bind(const void* blob_data, int sz)
{
    op([=](auto p) {
       sqlite3_bind_blob(p,
                         bind_ndx++,
                         blob_data,
                         sz,
                         SQLITE_TRANSIENT);
    });
    return *this;
}

DB::Statement& DB::Statement::bind(int n)
{
    op([=](auto p) {
       sqlite3_bind_int(p, bind_ndx++, n);
    });
    return *this;
}

DB::Statement& DB::Statement::bind(std::string text)
{
    op([=](auto p) {
       sqlite3_bind_text(p,
                         bind_ndx++,
                         text.c_str(),
                         text.size() + 1,
                         SQLITE_TRANSIENT);
    });
    return *this;
}

DB::Statement& DB::Statement::col(int ndx, const double*& blob_p)
{
    op([&](auto p) {
       blob_p =
           reinterpret_cast<const double*>(sqlite3_column_blob(p, ndx));
    });
    return *this;
}

DB::Statement& DB::Statement::col(int ndx, int64_t& n)
{
    op([&](auto p) {
       n = sqlite3_column_int64(p, ndx);
    });
    return *this;
}

DB::Statement& DB::Statement::col(int ndx, std::string& s)
{
    op([&](auto p) {
       const unsigned char* s_p = sqlite3_column_text(p, ndx);
       if (s_p) {
           s = std::string(reinterpret_cast<char*>(const_cast<unsigned char*>(s_p)));
       }
    });
    return *this;
}

int DB::Statement::col_sz(int ndx)
{
    int out;
    op([&](auto p) {
       out = sqlite3_column_bytes(p, ndx);
    });
    return out;
}

std::string DB::Statement::source() const
{
    return src;
}
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

    if (stmts_p->count(src) == 0) {
        check_err<prep_stmt_error>(sqlite3_prepare_v2(db_p,
                                                      src.c_str(),
                                                      src.length() + 1,
                                                      &stmtp,
                                                      &z_tail),
                                   src);

        stmts_p->emplace(src, stmtp);
    }

    return {stmts_p, src};
}

void DB::delete_stmt::operator()(sqlite3_stmt* p)
{
    // this returns an error code if the most recent evaluation of the statement
    // failed, but i'm not sure that's something we really need to worry about
    // in its destructor
    sqlite3_finalize(p);
}

DB::sqlite_error::sqlite_error(int sqlite_errcode,
             std::string action,
             std::string extra)
    : runtime_error {sqlite_errmsg(sqlite_errcode, action, extra)}
{}

std::string DB::sqlite_error::sqlite_errmsg(int sqlite_errcode,
                          std::string action,
                          std::string extra)
{
    std::stringstream s;

    s << "sqlite3 failed"
      << " to " << action
      << ": "
      << sqlite3_errstr(sqlite_errcode)
      << " (" << sqlite_errcode << ")";

    if (!extra.empty()) {
        s << "\n" << extra;
    }

    return s.str();
}

DB::open_db_error::open_db_error(int sqlite_errcode, std::filesystem::path pth)
    : sqlite_error {sqlite_errcode,
                    "open the database at " + pth.string()}
{}

DB::stmt_error::stmt_error(int sqlite_errcode,
                           std::string action,
                           std::string source)
    : sqlite_error {sqlite_errcode,
                    action + " a statement",
                    "source: " + source}
{}

DB::blob_error::blob_error(int sqlite_errcode,
                           std::string action)
    : sqlite_error {sqlite_errcode, action +  " a blob"}
{}

DB::prep_stmt_error::prep_stmt_error(int sqlite_errcode, std::string source)
    : stmt_error {sqlite_errcode, "prepare", source}
{}

DB::step_stmt_error::step_stmt_error(int sqlite_errcode, std::string source)
    : stmt_error {sqlite_errcode, "evaluate", source}
{}

DB::open_blob_error::open_blob_error(int sqlite_errcode)
    : blob_error {sqlite_errcode, "open"}
{}

DB::write_blob_error::write_blob_error(int sqlite_errcode)
    : blob_error {sqlite_errcode, "write"}
{}

DB::read_blob_error::read_blob_error(int sqlite_errcode)
    : blob_error {sqlite_errcode, "read"}
{}

DB::close_blob_error::close_blob_error(int sqlite_errcode)
    : blob_error {sqlite_errcode, "close"}
{}
