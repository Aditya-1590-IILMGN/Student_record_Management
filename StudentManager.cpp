#include "StudentManager.h"
#include "InputValidator.h"
#include <iomanip>
#include <iostream>

StudentManager::StudentManager(Database& dbRef) : db(dbRef) {}

void StudentManager::printHeader() const {
    std::cout << "+--------+--------------------------+--------------------+----------+\n";
    std::cout << "| " << std::left << std::setw(6) << "Roll"
              << " | " << std::setw(24) << "Student Name"
              << " | " << std::setw(18) << "Course"
              << " | " << std::setw(8) << "Semester" << " |\n";
    std::cout << "+--------+--------------------------+--------------------+----------+\n";
}

void StudentManager::printRow(const Student& st) const {
    std::string nm = st.name.size() > 24 ? st.name.substr(0, 21) + "..." : st.name;
    std::string crs = st.course.size() > 18 ? st.course.substr(0, 15) + "..." : st.course;

    std::cout << "| " << std::left << std::setw(6) << st.roll
              << " | " << std::setw(24) << nm
              << " | " << std::setw(18) << crs
              << " | " << std::setw(8) << st.sem << " |\n";
}

std::optional<Student> StudentManager::findByRoll(int roll, bool& ok) {
    ok = false;
    if (!db.connected()) {
        std::cerr << "Database connection is closed.\n";
        return std::nullopt;
    }

    const char* q = "SELECT roll, name, course, semester FROM Students WHERE roll = ?;";
    Statement stmt(db.raw(), q);
    if (!stmt.ok()) {
        std::cerr << "Query prepare error: " << db.errText() << "\n";
        return std::nullopt;
    }

    if (sqlite3_bind_int(stmt.ptr(), 1, roll) != SQLITE_OK) {
        std::cerr << "Bind error: " << db.errText() << "\n";
        return std::nullopt;
    }

    int res = sqlite3_step(stmt.ptr());
    if (res == SQLITE_ROW) {
        Student st;
        st.roll = sqlite3_column_int(stmt.ptr(), 0);
        const unsigned char* colName = sqlite3_column_text(stmt.ptr(), 1);
        const unsigned char* colCourse = sqlite3_column_text(stmt.ptr(), 2);
        st.name = colName ? reinterpret_cast<const char*>(colName) : "";
        st.course = colCourse ? reinterpret_cast<const char*>(colCourse) : "";
        st.sem = sqlite3_column_int(stmt.ptr(), 3);
        ok = true;
        return st;
    }
    if (res == SQLITE_DONE) {
        ok = true;
        return std::nullopt;
    }
    std::cerr << "Query execution error: " << db.errText() << "\n";
    return std::nullopt;
}

void StudentManager::addStudent() {
    std::cout << "\n--- Add Student ---\n";
    Student st;

    if (!InputValidator::getInt("Roll Number (1 - 999999): ", 1, 999999, st.roll)) return;

    bool ok = false;
    auto prev = findByRoll(st.roll, ok);
    if (!ok) return;
    if (prev.has_value()) {
        std::cout << "Roll number " << st.roll << " already exists (" << prev->name << ").\n";
        return;
    }

    if (!InputValidator::getText("Student Name: ", st.name)) return;
    if (!InputValidator::getText("Course (e.g. B.Tech CSE, BCA): ", st.course)) return;
    if (!InputValidator::getInt("Semester (1 - 12): ", 1, 12, st.sem)) return;

    const char* q = "INSERT INTO Students (roll, name, course, semester) VALUES (?, ?, ?, ?);";
    Statement stmt(db.raw(), q);
    if (!stmt.ok()) {
        std::cerr << "Insert prepare error: " << db.errText() << "\n";
        return;
    }

    sqlite3_bind_int(stmt.ptr(), 1, st.roll);
    sqlite3_bind_text(stmt.ptr(), 2, st.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt.ptr(), 3, st.course.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt.ptr(), 4, st.sem);

    int res = sqlite3_step(stmt.ptr());
    if (res == SQLITE_DONE) {
        std::cout << "Student " << st.roll << " added.\n";
    } else if (res == SQLITE_CONSTRAINT) {
        std::cout << "Constraint error: " << db.errText() << "\n";
    } else {
        std::cerr << "Insert failed: " << db.errText() << "\n";
    }
}

void StudentManager::viewAll() {
    std::cout << "\n--- All Students ---\n";
    if (!db.connected()) {
        std::cerr << "Database connection is closed.\n";
        return;
    }

    const char* q = "SELECT roll, name, course, semester FROM Students ORDER BY roll ASC;";
    Statement stmt(db.raw(), q);
    if (!stmt.ok()) {
        std::cerr << "Select prepare error: " << db.errText() << "\n";
        return;
    }

    int total = 0;
    bool topDone = false;
    int res = 0;
    while ((res = sqlite3_step(stmt.ptr())) == SQLITE_ROW) {
        if (!topDone) {
            printHeader();
            topDone = true;
        }
        Student st;
        st.roll = sqlite3_column_int(stmt.ptr(), 0);
        const unsigned char* colName = sqlite3_column_text(stmt.ptr(), 1);
        const unsigned char* colCourse = sqlite3_column_text(stmt.ptr(), 2);
        st.name = colName ? reinterpret_cast<const char*>(colName) : "";
        st.course = colCourse ? reinterpret_cast<const char*>(colCourse) : "";
        st.sem = sqlite3_column_int(stmt.ptr(), 3);

        printRow(st);
        ++total;
    }

    if (res != SQLITE_DONE) {
        std::cerr << "Read error: " << db.errText() << "\n";
        return;
    }

    if (total == 0) {
        std::cout << "No records found.\n";
    } else {
        std::cout << "+--------+--------------------------+--------------------+----------+\n";
        std::cout << "Total: " << total << "\n";
    }
}

void StudentManager::searchStudent() {
    std::cout << "\n--- Search Student ---\n";
    int r = 0;
    if (!InputValidator::getInt("Enter Roll Number: ", 1, 999999, r)) return;

    bool ok = false;
    auto st = findByRoll(r, ok);
    if (!ok) return;

    if (!st.has_value()) {
        std::cout << "No student found with roll number " << r << ".\n";
        return;
    }

    printHeader();
    printRow(*st);
    std::cout << "+--------+--------------------------+--------------------+----------+\n";
}

void StudentManager::updateStudent() {
    std::cout << "\n--- Update Student ---\n";
    int r = 0;
    if (!InputValidator::getInt("Enter Roll Number: ", 1, 999999, r)) return;

    bool ok = false;
    auto cur = findByRoll(r, ok);
    if (!ok) return;

    if (!cur.has_value()) {
        std::cout << "No student found with roll number " << r << ".\n";
        return;
    }

    Student upd = *cur;
    std::cout << "Current -> Roll: " << upd.roll
              << " | Name: " << upd.name
              << " | Course: " << upd.course
              << " | Semester: " << upd.sem << "\n";

    std::cout << "1. Name\n"
              << "2. Course\n"
              << "3. Semester\n"
              << "4. All Fields\n";

    int opt = 0;
    if (!InputValidator::getInt("Choice (1 - 4): ", 1, 4, opt)) return;

    if (opt == 1 || opt == 4) {
        if (!InputValidator::getText("New Name: ", upd.name)) return;
    }
    if (opt == 2 || opt == 4) {
        if (!InputValidator::getText("New Course: ", upd.course)) return;
    }
    if (opt == 3 || opt == 4) {
        if (!InputValidator::getInt("New Semester (1 - 12): ", 1, 12, upd.sem)) return;
    }

    const char* q = "UPDATE Students SET name = ?, course = ?, semester = ? WHERE roll = ?;";
    Statement stmt(db.raw(), q);
    if (!stmt.ok()) {
        std::cerr << "Update prepare error: " << db.errText() << "\n";
        return;
    }

    sqlite3_bind_text(stmt.ptr(), 1, upd.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt.ptr(), 2, upd.course.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt.ptr(), 3, upd.sem);
    sqlite3_bind_int(stmt.ptr(), 4, upd.roll);

    int res = sqlite3_step(stmt.ptr());
    if (res == SQLITE_DONE) {
        if (sqlite3_changes(db.raw()) > 0) {
            std::cout << "Record " << upd.roll << " updated.\n";
        } else {
            std::cout << "No changes saved for roll " << upd.roll << ".\n";
        }
    } else {
        std::cerr << "Update failed: " << db.errText() << "\n";
    }
}

void StudentManager::deleteStudent() {
    std::cout << "\n--- Delete Student ---\n";
    int r = 0;
    if (!InputValidator::getInt("Enter Roll Number: ", 1, 999999, r)) return;

    bool ok = false;
    auto cur = findByRoll(r, ok);
    if (!ok) return;

    if (!cur.has_value()) {
        std::cout << "No student found with roll number " << r << ".\n";
        return;
    }

    std::cout << "Found -> Roll: " << cur->roll
              << " | Name: " << cur->name
              << " | Course: " << cur->course
              << " | Semester: " << cur->sem << "\n";

    bool yes = false;
    if (!InputValidator::getConfirm("Delete this record? (y/n): ", yes)) return;
    if (!yes) {
        std::cout << "Cancelled.\n";
        return;
    }

    const char* q = "DELETE FROM Students WHERE roll = ?;";
    Statement stmt(db.raw(), q);
    if (!stmt.ok()) {
        std::cerr << "Delete prepare error: " << db.errText() << "\n";
        return;
    }

    sqlite3_bind_int(stmt.ptr(), 1, r);

    int res = sqlite3_step(stmt.ptr());
    if (res == SQLITE_DONE) {
        if (sqlite3_changes(db.raw()) > 0) {
            std::cout << "Record " << r << " deleted.\n";
        } else {
            std::cout << "Record already deleted.\n";
        }
    } else {
        std::cerr << "Delete failed: " << db.errText() << "\n";
    }
}
