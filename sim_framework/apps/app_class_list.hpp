#ifndef APP_CLASS_LIST_HPP
#define APP_CLASS_LIST_HPP

// C++ cannot look up a class from a string, so a project lists every
// class the sim may use with string name so that the code can find it
//
//   AppClassList available_apps = {
//       app_class<ClassA>("ClassA"),
//       app_class<ClassA>("ClassB"),
//   };
//
//   SimManager sim(path_to_sim_config, available_apps);

#include <array>
#include <cstddef>
#include <initializer_list>
#include <string>

#include "../config_parsing/app_manifest.hpp"
#include "../sim_main_loop/sim_structs.hpp"
#include "../sim_config.hpp"

struct AppClassFactory {
    std::string class_name;
    SimAppPrototype (*create_prototype)(const AppManifestEntry& entry) = nullptr;
};

template<typename AppType>
SimAppPrototype build_app_prototype(const AppManifestEntry& entry);

template<typename AppType>
AppClassFactory app_class(const std::string& class_name);

class AppClassList {
public:
    AppClassList(std::initializer_list<AppClassFactory> listed_classes);
    bool is_listed(const std::string& class_name) const;
    const AppClassFactory* find(const std::string& class_name) const;
    std::string list_class_names() const;

private:
    std::array<AppClassFactory, SimConfig::max_app_number> factories;
    std::size_t count = 0;
};

#include "app_class_list.tpp"

#endif
