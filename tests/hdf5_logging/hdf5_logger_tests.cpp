#include "..\src\logger\hdf5_logger.hpp"

#include <string>
#include <gtest/gtest.h>
#include <filesystem>

TEST(HDF5_LOGGER, TestClassConstructionWithValidPath) {
    std::filesystem::path path_to_build_dir        = std::filesystem::current_path() ;
    std::filesystem::path path_to_personal_project = path_to_build_dir.parent_path().parent_path();
    std::filesystem::path path_to_current_dir      = path_to_personal_project / "tests" / "hdf5_logging";

    std::string file_name = "unit_test_file.hdf5";
    std::string full_path = (path_to_current_dir / file_name).string();

    EXPECT_NO_THROW(HDF5Logger logger(file_name, path_to_current_dir));
};


TEST(HDF5_LOGGER, TestClassConstructionWithInvalidPath) {
    std::filesystem::path path_to_build_dir        = std::filesystem::current_path() ;
    std::filesystem::path path_to_personal_project = path_to_build_dir.parent_path().parent_path();
    std::filesystem::path wrong_path               = path_to_personal_project / "hdf5_logging";

    std::string file_name = "unit_test_file.hdf5";
    std::string full_path = (wrong_path / file_name).string();

    EXPECT_ANY_THROW(HDF5Logger logger(file_name, wrong_path));
};


TEST(HDF5_LOGGER, TestCreatingClosingAndOpeningFile) {
    std::filesystem::path path_to_build_dir        = std::filesystem::current_path() ;
    std::filesystem::path path_to_personal_project = path_to_build_dir.parent_path().parent_path();
    std::filesystem::path path_to_current_dir      = path_to_personal_project / "tests" / "hdf5_logging";

    std::string file_name = "unit_test_file.hdf5";
    std::string full_path = (path_to_current_dir / file_name).string();

    if (std::filesystem::exists(full_path)) {
        std::filesystem::remove(full_path);
    }

    HDF5Logger logger(file_name, path_to_current_dir);
    
    logger.create_file();
    EXPECT_TRUE(logger.is_file_open());
    EXPECT_TRUE(std::filesystem::exists(full_path));
    
    logger.close_file();
    EXPECT_FALSE(logger.is_file_open());

    logger.open_file();
    EXPECT_TRUE(logger.is_file_open());

    logger.close_file();
    std::filesystem::remove(full_path);
};
