#ifndef SIM_INCLUDES_HPP
#define SIM_INCLUDES_HPP

// Single header for projects using the sim framework: brings in the manager
// that runs a simulation, the base class apps inherit from, the port registry
// apps declare their IO with, and the class list a project uses to tell the
// manager which app classes its manifest may instantiate.

#include "sim_core/sim_manager.hpp"
#include "sim_core/app_class_list.hpp"
#include "generic_apps/sim_app_base.hpp"
#include "messaging/io_registry.hpp"

#endif
