#include "Database.h"
#include <iostream>

Statement::Statement(sqlite3* conn, const std::string& query) {
    if (conn) {
        if (sqlite3_prepare_v2(conn, query.c_str(), -1, &st, nullptr) != SQLITE_OK) {
            st = nullptr;
        }
    }
}

Statement::~Statement() {
    if (st) {
        sqlite3_finalize(st);
        st = nullptr;
    }
}

Database::Database(const std::string& file) : fileName(file) {
    if (sqlite3_open(fileName.c_str(), &conn) != SQLITE_OK) {
        std::cerr << "Error opening database '" << fileName << "'";
        if (conn) {
            std::cerr << ": " << sqlite3_errmsg(conn);
            sqlite3_close(conn);
            conn = nullptr;
        }
        std::cerr << "\n";
        return;
    }
    sqlite3_busy_timeout(conn, 3000);
    char* msg = nullptr;
    if (sqlite3_exec(conn, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &msg) != SQLITE_OK) {
        if (msg) sqlite3_free(msg);
    }
}

Database::~Database() {
    if (conn) {
        sqlite3_close(conn);
        conn = nullptr;
    }
}

std::string Database::errText() const {
    if (!conn) return "Database not open.";
    const char* msg = sqlite3_errmsg(conn);
    return msg ? msg : "Unknown SQLite error.";
}

bool Database::initTables() {
    if (!conn) {
        std::cerr << "Database is not open.\n";
        return false;
    }

    const char* q =
        "CREATE TABLE IF NOT EXISTS Students ("
        "    roll INTEGER PRIMARY KEY CHECK (roll > 0),"
        "    name TEXT NOT NULL CHECK (length(trim(name)) > 0),"
        "    course TEXT NOT NULL CHECK (length(trim(course)) > 0),"
        "    semester INTEGER NOT NULL CHECK (semester >= 1 AND semester <= 12)"
        ");";

    char* msg = nullptr;
    if (sqlite3_exec(conn, q, nullptr, nullptr, &msg) != SQLITE_OK) {
        std::cerr << "Table creation failed: " << (msg ? msg : sqlite3_errmsg(conn)) << "\n";
        if (msg) sqlite3_free(msg);
        return false;
    }
    return true;
}
