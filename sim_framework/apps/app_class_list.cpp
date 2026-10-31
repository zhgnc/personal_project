#include "app_class_list.hpp"

#include <stdexcept>

AppClassList::AppClassList(std::initializer_list<AppClassFactory> listed_classes) {
    bool too_many_classes = listed_classes.size() > SimConfig::max_app_number;

    if (too_many_classes == true) {
        throw std::logic_error(SimConfig::capacity_message("[AppClassList] number of app classes",
                                                           "max_app_number", SimConfig::max_app_number));
    }

    for (const AppClassFactory& factory : listed_classes) {
        if (is_listed(factory.class_name) == true) {
            throw std::logic_error("[AppClassList] app class '" + factory.class_name + "' is listed twice");
        }

        factories[count] = factory;
        count++;
    }
}

bool AppClassList::is_listed(const std::string& class_name) const {
    return find(class_name) != nullptr;
}

// Returns the entry with this class name or a nullptr if no existing entry is found
const AppClassFactory* AppClassList::find(const std::string& class_name) const {
    for (std::size_t i = 0; i < count; i++) {
        bool class_matches = factories[i].class_name == class_name;

        if (class_matches == true) {
            return &factories[i];
        }
    }

    return nullptr;
}

std::string AppClassList::list_class_names() const {
    bool list_is_empty = count == 0;

    if (list_is_empty == true) {
        return "none";
    }

    std::string joined = factories[0].class_name;

    for (std::size_t i = 1; i < count; i++) {
        joined += ", " + factories[i].class_name;
    }

    return joined;
}
