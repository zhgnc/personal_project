#ifndef APP_CLASS_LIST_HPP
#define APP_CLASS_LIST_HPP

// C++ cannot look up a class from a string, so a project lists every app
// class its manifest may use, each with the name the manifest's `class:` key
// writes for it:
//
//   AppClassList available_apps = {
//       app_class<FakeDynamicsSimApp>("FakeDynamicsSimApp"),
//       app_class<GyroSimApp>("GyroSimApp"),
//   };
//
//   SimManager sim(path_to_sim_config, available_apps);

#include <array>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "sim_app_base.hpp"
#include "../config_parsing/app_manifest.hpp"
#include "../sim_main_loop/sim_structs.hpp"
#include "../sim_config.hpp"

struct AppClassFactory {
    std::string class_name;
    SimAppPrototype (*create_prototype)(const AppManifestEntry& entry) = nullptr;
};

template<typename AppType>
AppClassFactory app_class(const std::string& class_name) {
    static_assert(std::is_base_of_v<SimAppBase, AppType>, "AppType must derive from SimAppBase");

    AppClassFactory factory;

    factory.class_name       = class_name;
    factory.create_prototype = [](const AppManifestEntry& entry) -> SimAppPrototype {
        return SimAppPrototype(AppType(entry.instance_name, entry.rate_hz, entry.priority, entry.config_path));
    };

    return factory;
}

class AppClassList {
public:
    AppClassList(std::initializer_list<AppClassFactory> listed_classes) {
        bool too_many_classes = listed_classes.size() > SimConfig::max_app_number;

        if (too_many_classes == true) {
            throw std::logic_error(SimConfig::capacity_message("[AppClassList] number of app classes",
                                                               "max_app_number", SimConfig::max_app_number));
        }

        for (const AppClassFactory& factory : listed_classes) {
            bool class_already_listed = find(factory.class_name) != nullptr;

            if (class_already_listed == true) {
                throw std::logic_error("[AppClassList] app class '" + factory.class_name + "' is listed twice");
            }

            factories[count] = factory;
            count++;
        }
    }

    // nullptr when the class is not in the list
    const AppClassFactory* find(const std::string& class_name) const {
        for (std::size_t i = 0; i < count; i++) {
            bool class_matches = factories[i].class_name == class_name;

            if (class_matches == true) {
                return &factories[i];
            }
        }

        return nullptr;
    }

    std::string joined_class_names() const {
        std::string joined;

        for (std::size_t i = 0; i < count; i++) {
            joined += (i == 0 ? "" : ", ") + factories[i].class_name;
        }

        return count == 0 ? "none" : joined;
    }

private:
    std::array<AppClassFactory, SimConfig::max_app_number> factories;
    std::size_t count = 0;
};

#endif
