#ifndef Qf1d495741254da89069133509f0fdd0
#define Qf1d495741254da89069133509f0fdd0

#include "exceptions.hpp"

#include <sqlite3.h>

#include <filesystem>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

// maybe this should be a template over classes for individual tables?
class DB {
public:
    typedef sqlite3_stmt stmt;
    typedef std::string  stmt_src;
    typedef int64_t      id_t; // largest form of integer in sqlite, typical
                               // primary key type
    static constexpr id_t no_id = -1;

    struct delete_stmt {
        void operator()(stmt*);
    };

    typedef std::unique_ptr<stmt, delete_stmt> stmt_up;
    typedef std::unordered_map<stmt_src, stmt_up> stmts_col;

    typedef std::shared_ptr<stmts_col> stmts_col_p;

    struct Statement {
        Statement(stmts_col_p instmts_p, stmt_src insrc)
            : stmts_p {instmts_p},
              src {insrc}
        {};

        // i don't think this is async-safe atm
        void op(std::function<void(stmt*)> fn)
        {
            auto p = stmts_p->at(src).release();
            fn(p);
            stmts_p->at(src).reset(p);
        }

        auto step()
        {
            op([this](auto p) {
                check_err<step_stmt_error>(sqlite3_step(p), source());
            });
            bind_ndx = 1;
            return *this;
        }

        bool has_data()
        {
            bool out;
            op([&out](auto p) {
                out = (sqlite3_data_count(p) != 0);
            });
            return out;
        }

        auto bind(const void* blob_data, int sz)
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

        auto bind(int n)
        {
            op([=](auto p) {
               sqlite3_bind_int(p, bind_ndx++, n);
            });
            return *this;
        }

        auto bind(std::string text)
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

        auto col(int ndx, const double*& blob_p)
        {
            op([&](auto p) {
               blob_p =
                   reinterpret_cast<const double*>(sqlite3_column_blob(p, ndx));
            });
            return *this;
        }

        auto col(int ndx, int64_t& n)
        {
            op([&](auto p) {
               n = sqlite3_column_int64(p, ndx);
            });
            return *this;
        }

        auto col(int ndx, std::string& s)
        {
            op([&](auto p) {
               const unsigned char* s_p = sqlite3_column_text(p, ndx);
               if (s_p) {
                   s = std::string(reinterpret_cast<char*>(const_cast<unsigned char*>(s_p)));
               }
            });
            return *this;
        }

        int col_sz(int ndx)
        {
            int out;
            op([&](auto p) {
               out = sqlite3_column_bytes(p, ndx);
            });
            return out;
        }

        std::string source() const
        {
            return src;
        }

    private:
        stmts_col_p stmts_p;
        stmt_src src;
        int bind_ndx = 1;
    };

    class sqlite_error : public runtime_error {
    public:
        std::string sqlite_errmsg(int sqlite_errcode,
                                  std::string action,
                                  std::string extra = "")
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

        sqlite_error(int sqlite_errcode,
                     std::string action,
                     std::string extra = "")
            : runtime_error {sqlite_errmsg(sqlite_errcode, action, extra)}
        {}

    };

    class open_db_error : public sqlite_error {
    public:
        open_db_error(int sqlite_errcode, std::filesystem::path pth)
            : sqlite_error {sqlite_errcode,
                            "open the database at " + pth.string()}
        {}
    };

    class stmt_error : public sqlite_error {
    public:
        stmt_error(int sqlite_errcode, std::string action, std::string source)
            : sqlite_error {sqlite_errcode,
                            action + " a statement",
                            "source: " + source}
        {}
    };

    class prep_stmt_error : public stmt_error {
    public:
        prep_stmt_error(int sqlite_errcode, std::string source)
            : stmt_error {sqlite_errcode, "prepare", source}
        {}
    };

    class step_stmt_error : public stmt_error {
    public:
        step_stmt_error(int sqlite_errcode, std::string source)
            : stmt_error {sqlite_errcode, "evaluate", source}
        {}
    };

    template<typename T = sqlite_error>
    static void check_err(int result,
                          std::vector<int> expected = {SQLITE_OK,
                                                       SQLITE_ROW,
                                                       SQLITE_DONE})
    {
        for (const auto& expected_code : expected) {
            if (result == expected_code) {
                return;
            }
        }

        throw T(result);
    }

    template<typename T = sqlite_error>
    static void check_err(int result,
                          int expected)
    {
        if (result != expected) {
            throw T(result);
        }
    }

    template<typename T = sqlite_error>
    static void check_err(int result,
                          std::filesystem::path pth,
                          std::vector<int> expected = {SQLITE_OK,
                                                       SQLITE_ROW,
                                                       SQLITE_DONE})
    {
        for (const auto& expected_code : expected) {
            if (result == expected_code) {
                return;
            }
        }

        throw T(result, pth);
    }

    template<typename T = sqlite_error>
    static void check_err(int result,
                          std::string s,
                          std::vector<int> expected = {SQLITE_OK,
                                                       SQLITE_ROW,
                                                       SQLITE_DONE})
    {
        for (const auto& expected_code : expected) {
            if (result == expected_code) {
                return;
            }
        }

        throw T(result, s);
    }

    DB(std::filesystem::path dbfile);

    DB(const DB&) = delete;
    DB& operator=(const DB&) = delete;

    DB(DB&&) = delete;
    DB& operator=(DB&&) = delete;

    ~DB() noexcept;

    /* cached */
    Statement prep_stmt(std::string stmt);

    id_t last_id() { return sqlite3_last_insert_rowid(db_p); }

private:
    sqlite3* db_p = nullptr;

private:
    stmts_col_p stmts_p = std::make_shared<stmts_col>();
};

#endif
