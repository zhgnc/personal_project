#ifndef DATASET_OVERRIDES_HPP
#define DATASET_OVERRIDES_HPP

#include "../../src/sim_data_logger/dataset_base.hpp"
#include "../../external/hdf5/include/H5Cpp.h"
#include "../../src/math_utilities/math.hpp"
#include "../../src/sim_data_logger/cpp_to_hdf5_type_mapping.hpp"

#include <memory>
#include <type_traits>
#include <array>
#include <string>

// DataTraits is a compile time struc that defines some parameters required for hdf5 logging.
// Used to deduce number of dimensions, dimension sizes, and data pointer for HDF5 writes for
// the follow c++ types which includes my custom math library
template<typename T>
struct DataTraits {
    static constexpr int num_dimensions = 0;
    static std::array<hsize_t, 0> dimension_sizes();
    static const void* data_ptr(const T* value);
};

template<typename T, size_t N>
struct DataTraits<vector<T, N>> {
    static constexpr int num_dimensions = 1;
    static std::array<hsize_t, 1> dimension_sizes();
    static const void* data_ptr(const vector<T, N>* vector);
};

template<typename T, size_t R, size_t C>
struct DataTraits<matrix<T, R, C>> {
    static constexpr int num_dimensions = 2;
    static std::array<hsize_t, 2> dimension_sizes();
    static const void* data_ptr(const matrix<T, R, C>* matrix);
};





template<typename T>
class DatasetOverrides : public DatasetBase {
public:
    DatasetOverrides(const std::string& name,
                         const std::string& full_group_path,
                         std::shared_ptr<T> data_pointer,
                         H5::H5File& file,
                         int record_rate_hz,
                         int buffer_length);

    void create_dataset();
    void log_if_needed(const uint32_t& current_sim_time_use) override;
    void flush_buffer() override;

private:
    std::shared_ptr<T> data_ptr;
};

#include "../../src/sim_data_logger/dataset_overrides.tpp"  // Template implementations

#endif