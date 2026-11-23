#ifndef STUDENT_MANAGER_H
#define STUDENT_MANAGER_H

#include "Database.h"
#include "Student.h"
#include <optional>

class StudentManager {
public:
    explicit StudentManager(Database& dbRef);

    void addStudent();
    void viewAll();
    void searchStudent();
    void updateStudent();
    void deleteStudent();

private:
    Database& db;

    std::optional<Student> findByRoll(int roll, bool& ok);
    void printHeader() const;
    void printRow(const Student& st) const;
};

#endif
