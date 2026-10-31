#ifndef APP_CLASS_LIST_TPP
#define APP_CLASS_LIST_TPP

#include "app_class_list.hpp"

#include <type_traits>
#include <utility>

#include "sim_app_base.hpp"

// Called by SimManager once per enabled manifest entry of this class
template<typename AppType>
SimAppPrototype build_app_prototype(const AppManifestEntry& entry) {
    AppType app(entry.instance_name, entry.rate_hz, entry.priority, entry.config_path);

    return SimAppPrototype(std::move(app));
}

template<typename AppType>
AppClassFactory app_class(const std::string& class_name) {
    static_assert(std::is_base_of_v<SimAppBase, AppType>, "AppType must derive from SimAppBase");

    AppClassFactory factory;

    factory.class_name       = class_name;
    factory.create_prototype = build_app_prototype<AppType>;

    return factory;
}

#endif
