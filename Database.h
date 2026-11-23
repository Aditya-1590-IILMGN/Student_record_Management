#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <sqlite3.h>

class Statement {
public:
    Statement(sqlite3* conn, const std::string& query);
    ~Statement();

    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;

    bool ok() const { return st != nullptr; }
    sqlite3_stmt* ptr() const { return st; }

private:
    sqlite3_stmt* st = nullptr;
};

class Database {
public:
    explicit Database(const std::string& file);
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    bool connected() const { return conn != nullptr; }
    sqlite3* raw() const { return conn; }
    std::string errText() const;

    bool initTables();

private:
    sqlite3* conn = nullptr;
    std::string fileName;
};

#endif
