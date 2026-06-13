#ifndef FILE_PATH_HELPER_FUNCTIONS_HPP
#define FILE_PATH_HELPER_FUNCTIONS_HPP

#include <filesystem>
#include <stdexcept>
#include <string>

// Checks that the PROJECT_SOURCE_DIR variable is defined by CMake
#ifndef PROJECT_SOURCE_DIR
#error "PROJECT_SOURCE_DIR is not defined! Make sure CMake sets it."
#endif

// Returns the absolute project root path if the PROJECT_SOURCE_DIR is defined by CMake
inline std::string get_project_root() {
    return std::filesystem::canonical(PROJECT_SOURCE_DIR).string();
}

// Returns an absolute path of a relative file path to the project root
inline std::string get_absolute_path(const std::string& path_relative_to_root_dir)
{
    std::filesystem::path path(path_relative_to_root_dir);

    if (!path.is_absolute()) {
        path = std::filesystem::path(PROJECT_SOURCE_DIR) / path;
    }

    return std::filesystem::absolute(path).string();
}

#endif
