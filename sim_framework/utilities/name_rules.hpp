#ifndef NAME_RULES_HPP
#define NAME_RULES_HPP

// The naming rule the whole framework shares.
//
// App names, port names, and anything else addressed from a config file use
// the same character set, so a name reads identically in yaml, in a telemetry
// column, and in an error message. violation() reports which rule was broken
// and message() turns that into the second half of "name 'x' is invalid: ...",
// so every caller words the same mistake the same way.

#include <string>

enum class NameViolation {
    None,
    Empty,
    ContainsSpace,
    ContainsInvalidCharacter
};

class NameRules {
public:
    static NameViolation violation(const std::string& name) {
        if (name.empty() == true) {
            return NameViolation::Empty;
        }

        for (char character : name) {
            bool valid = (character >= 'a' && character <= 'z') ||
                         (character >= '0' && character <= '9') ||
                         character == '_';

            if (valid == false) {
                // Spaces get their own value because they are the easy mistake
                // to make in a yaml file and the hard one to see afterwards
                if (character == ' ') {
                    return NameViolation::ContainsSpace;
                }

                return NameViolation::ContainsInvalidCharacter;
            }
        }

        return NameViolation::None;
    }

    static std::string message(NameViolation violation) {
        std::string rule = "; names must use only lowercase letters, digits, and underscores";

        switch (violation) {
            case NameViolation::None:                     return "";
            case NameViolation::Empty:                    return "name is empty";
            case NameViolation::ContainsSpace:            return "contains a space" + rule;
            case NameViolation::ContainsInvalidCharacter: return "contains an invalid character" + rule;
        }

        return "";
    }
};

#endif
