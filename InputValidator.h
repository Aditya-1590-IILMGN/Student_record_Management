#ifndef INPUT_VALIDATOR_H
#define INPUT_VALIDATOR_H

#include <string>

namespace InputValidator {
    std::string trim(const std::string& str);
    bool getInt(const std::string& label, int low, int high, int& out);
    bool getText(const std::string& label, std::string& out);
    bool getConfirm(const std::string& label, bool& yes);
}

#endif
