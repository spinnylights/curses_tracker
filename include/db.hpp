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
        Statement(stmts_col_p instmts_p, stmt_src insrc);

        // i don't think this is async-safe atm
        void op(std::function<void(stmt*)> fn);

        struct Statement& step();

        bool has_data();

        struct Statement& bind(const void* blob_data, int sz);

        struct Statement& bind(int n);

        struct Statement& bind(std::string text);

        struct Statement& col(int ndx, const double*& blob_p);

        struct Statement& col(int ndx, int64_t& n);

        struct Statement& col(int ndx, std::string& s);

        int col_sz(int ndx);

        std::string source() const;

    private:
        stmts_col_p stmts_p;
        stmt_src src;
        int bind_ndx = 1;
    };

    class sqlite_error : public runtime_error {
    public:
        sqlite_error(int sqlite_errcode,
                     std::string action,
                     std::string extra = "");

        std::string sqlite_errmsg(int sqlite_errcode,
                                  std::string action,
                                  std::string extra = "");

    };

    class open_db_error : public sqlite_error {
    public:
        open_db_error(int sqlite_errcode, std::filesystem::path pth);
    };

    class stmt_error : public sqlite_error {
    public:
        stmt_error(int sqlite_errcode, std::string action, std::string source);
    };

    class blob_error : public sqlite_error {
    public:
        blob_error(int sqlite_errcode, std::string action);
    };

    class prep_stmt_error : public stmt_error {
    public:
        prep_stmt_error(int sqlite_errcode, std::string source);
    };

    class step_stmt_error : public stmt_error {
    public:
        step_stmt_error(int sqlite_errcode, std::string source);
    };

    class open_blob_error : public blob_error {
    public:
        open_blob_error(int sqlite_errcode);
    };

    class write_blob_error : public blob_error {
    public:
        write_blob_error(int sqlite_errcode);
    };

    class read_blob_error : public blob_error {
    public:
        read_blob_error(int sqlite_errcode);
    };

    class close_blob_error : public blob_error {
    public:
        close_blob_error(int sqlite_errcode);
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
