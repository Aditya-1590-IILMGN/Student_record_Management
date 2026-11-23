#include "Database.h"
#include "InputValidator.h"
#include "StudentManager.h"
#include <iostream>
#include <string>

static void showMenu() {
    std::cout << "\n======================================================\n";
    std::cout << "          STUDENT RECORD MANAGEMENT SYSTEM\n";
    std::cout << "======================================================\n";
    std::cout << "  1. Add Student\n";
    std::cout << "  2. View All Students\n";
    std::cout << "  3. Search Student by Roll Number\n";
    std::cout << "  4. Update Student Details\n";
    std::cout << "  5. Delete Student Record\n";
    std::cout << "  6. Exit\n";
    std::cout << "------------------------------------------------------\n";
}

int main() {
    Database db("students.db");
    if (!db.connected() || !db.initTables()) {
        return 1;
    }

    StudentManager mgr(db);

    while (true) {
        showMenu();
        int ch = 0;
        if (!InputValidator::getInt("Select option (1 - 6): ", 1, 6, ch)) {
            break;
        }

        switch (ch) {
            case 1: mgr.addStudent(); break;
            case 2: mgr.viewAll(); break;
            case 3: mgr.searchStudent(); break;
            case 4: mgr.updateStudent(); break;
            case 5: mgr.deleteStudent(); break;
            case 6:
                std::cout << "Exiting...\n";
                return 0;
            default:
                break;
        }
    }

    return 0;
}
