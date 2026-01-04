#ifndef DATASET_OVERRIDES_HPP
#define DATASET_OVERRIDES_HPP

#include "../../src/data_logging/dataset_base.hpp"
#include "../../external/hdf5/include/H5Cpp.h"
#include "../../src/math_utilities/math.hpp"
#include "../../src/data_logging/cpp_to_hdf5_type_mapping.hpp"

#include <memory>
#include <type_traits>
#include <array>
#include <string>

// DataTraits is a compile-time struct used to deduce number of dimensions, size of dimesnions, and data pointers
// for HDF5 dataset writes. Specializations handle built-in types, as well as custom math types in "math_utilities/"
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
    static const void* data_ptr(const vector<T, N>* vec);
};

template<typename T, size_t R, size_t C>
struct DataTraits<matrix<T, R, C>> {
    static constexpr int num_dimensions = 2;
    static std::array<hsize_t, 2> dimension_sizes();
    static const void* data_ptr(const matrix<T, R, C>* mat);
};





template<typename T, std::size_t buffer_length>
class DatasetOverrides : public DatasetBase {
public:
    DatasetOverrides(const std::string& name,
                     const std::string& full_group_path,
                     std::shared_ptr<const T> data_pointer,
                     std::shared_ptr<H5::H5File> file,
                     const double record_rate_hz);

    void create_dataset();
    void log_if_needed(const uint32_t& current_sim_time_use) override;
    void flush_buffer() override;

private:
    std::shared_ptr<const T> data_ptr;
    std::array<T, buffer_length> data_buffer;

    std::array<hsize_t, DataTraits<T>::num_dimensions + 1> old_dataset_size;
    std::array<hsize_t, DataTraits<T>::num_dimensions + 1> new_dataset_size;
    std::array<hsize_t, DataTraits<T>::num_dimensions + 1> new_data_shape;
};

#include "../../src/data_logging/dataset_overrides.tpp"  // Template implementations

#endif