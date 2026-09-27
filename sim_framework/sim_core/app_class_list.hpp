#ifndef APP_CLASS_LIST_HPP
#define APP_CLASS_LIST_HPP

// The bridge from a class name written in yaml to a C++ constructor.
//
// C++ keeps no runtime inventory of its classes, so the compiler must be told
// somewhere which classes a manifest may instantiate. A project states that
// once as an AppClassList; each entry mentions its class a single time and
// the matching name string is extracted from the type at compile time, so
// the two can never drift apart.

#include <array>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

#include "sim_config.hpp"
#include "sim_structs.hpp"
#include "app_manifest.hpp"

// Maps a class name from the app manifest to a function that constructs a
// prototype of that class. A plain function pointer (not std::function) so
// the fixed-size factory table involves no dynamic allocation
struct AppClassFactory {
    std::string class_name;
    SimAppPrototype (*create_prototype)(const AppManifestEntry& entry, int schedule_priority) = nullptr;
};

// Extracts the class name (e.g. "GyroSimApp") from the compiler's function
// signature at compile time, so app_class<T>() never needs a hand-written
// string that could drift from the real class name. Classes inside a
// namespace come out qualified ("my_project::GyroSimApp") — the manifest
// `class:` value must match what this returns.
template<typename AppType>
constexpr std::string_view class_name_of() {
#if defined(_MSC_VER) && !defined(__clang__) && !defined(__GNUC__)
    // __FUNCSIG__ looks like: "... class_name_of<class GyroSimApp>(void)"
    constexpr std::string_view signature = __FUNCSIG__;
    constexpr std::string_view marker    = "class_name_of<";

    std::size_t start = signature.find(marker) + marker.size();
    std::size_t stop  = signature.rfind(">(");

    std::string_view name = signature.substr(start, stop - start);

    if (name.starts_with("class "))  { name.remove_prefix(6); }
    if (name.starts_with("struct ")) { name.remove_prefix(7); }

    return name;
#else
    // __PRETTY_FUNCTION__ looks like: "... class_name_of() [with AppType = GyroSimApp; ...]"
    constexpr std::string_view signature = __PRETTY_FUNCTION__;
    constexpr std::string_view marker    = "AppType = ";

    std::size_t start = signature.find(marker) + marker.size();
    std::size_t stop  = signature.find_first_of(";]", start);

    return signature.substr(start, stop - start);
#endif
}

// One entry in the available-apps list. The class is mentioned exactly once:
// the manifest string it registers under is derived from the type itself
template<typename AppType>
AppClassFactory app_class() {
    static_assert(std::is_base_of_v<SimAppBase, AppType>, "AppType must derive from SimAppBase");
    static_assert(class_name_of<AppType>().empty() == false, "class name extraction failed");

    AppClassFactory factory;

    factory.class_name       = std::string(class_name_of<AppType>());
    factory.create_prototype = [](const AppManifestEntry& entry, int schedule_priority) -> SimAppPrototype {
        return SimAppPrototype(AppType(entry.instance_name, entry.rate_hz, schedule_priority, entry.config_path));
    };

    return factory;
}

// Every sim app class a project makes available to its manifest, e.g.:
//
//   const AppClassList attitude_filter_apps = {
//       app_class<FakeDynamicsSimApp>(),
//       app_class<GyroSimApp>(),
//   };
//
// The manifest's `class:` values select from this list; a class missing from
// it fails at startup with the registered-classes list and a suggestion.
struct AppClassList {
    std::array<AppClassFactory, SimConfig::max_app_number> factories;
    std::size_t count = 0;

    AppClassList(std::initializer_list<AppClassFactory> listed_classes) {
        if (listed_classes.size() > SimConfig::max_app_number) {
            throw std::logic_error("[app_class_list.hpp] Number of listed app classes exceeds `max_app_number` in sim_config.hpp which is "
                                   + std::to_string(SimConfig::max_app_number));
        }

        for (const AppClassFactory& factory : listed_classes) {
            factories[count] = factory;
            count++;
        }
    }
};

#endif
