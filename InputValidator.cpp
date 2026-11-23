#include "InputValidator.h"
#include <cctype>
#include <iostream>
#include <limits>
#include <sstream>

namespace InputValidator {

std::string trim(const std::string& str) {
    size_t i = 0;
    while (i < str.size() && std::isspace(static_cast<unsigned char>(str[i]))) {
        ++i;
    }
    size_t j = str.size();
    while (j > i && std::isspace(static_cast<unsigned char>(str[j - 1]))) {
        --j;
    }
    return str.substr(i, j - i);
}

bool getInt(const std::string& label, int low, int high, int& out) {
    while (true) {
        std::cout << label;
        std::string buf;
        if (!std::getline(std::cin, buf)) {
            if (std::cin.eof()) {
                std::cin.clear();
                return false;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Try again.\n";
            continue;
        }

        std::string s = trim(buf);
        if (s.empty()) {
            std::cout << "Input cannot be empty. Enter a number (" << low << " - " << high << ").\n";
            continue;
        }

        std::istringstream ss(s);
        long long num = 0;
        char ch = '\0';
        if (!(ss >> num) || (ss >> ch)) {
            std::cout << "Digits only please.\n";
            continue;
        }

        if (num < low || num > high) {
            std::cout << "Number must be between " << low << " and " << high << ".\n";
            continue;
        }

        out = static_cast<int>(num);
        return true;
    }
}

bool getText(const std::string& label, std::string& out) {
    while (true) {
        std::cout << label;
        std::string buf;
        if (!std::getline(std::cin, buf)) {
            if (std::cin.eof()) {
                std::cin.clear();
                return false;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Could not read input.\n";
            continue;
        }

        std::string s = trim(buf);
        if (s.empty()) {
            std::cout << "Field cannot be blank.\n";
            continue;
        }

        out = s;
        return true;
    }
}

bool getConfirm(const std::string& label, bool& yes) {
    while (true) {
        std::cout << label;
        std::string buf;
        if (!std::getline(std::cin, buf)) {
            std::cin.clear();
            return false;
        }
        std::string s = trim(buf);
        if (s == "y" || s == "Y" || s == "yes" || s == "YES" || s == "Yes") {
            yes = true;
            return true;
        }
        if (s == "n" || s == "N" || s == "no" || s == "NO" || s == "No") {
            yes = false;
            return true;
        }
        std::cout << "Please type y or n.\n";
    }
}

}
