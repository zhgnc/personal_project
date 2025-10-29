#ifndef DATASET_OVERRIDES_TPP
#define DATASET_OVERRIDES_TPP

#include "../../src/sim_data_logger/dataset_overrides.hpp"
#include "../../external/hdf5/include/H5Cpp.h"
#include "../../src/math_utilities/math.hpp"

#include <iostream>
#include <stdexcept>

// Implementation of DataTraits struct methods
template<typename T>
std::array<hsize_t, 0> DataTraits<T>::dimension_sizes() { return {}; }

template<typename T>
const void* DataTraits<T>::data_ptr(const T* value) { return value; }


template<typename T, size_t N>
std::array<hsize_t, 1> DataTraits<vector<T, N>>::dimension_sizes() { return {N}; }

template<typename T, size_t N>
const void* DataTraits<vector<T, N>>::data_ptr(const vector<T, N>* v) { return vector->data(); }


template<typename T, size_t R, size_t C>
std::array<hsize_t, 2> DataTraits<matrix<T, R, C>>::dimension_sizes() { return {R, C}; }

template<typename T, size_t R, size_t C>
const void* DataTraits<matrix<T, R, C>>::data_ptr(const matrix<T, R, C>* m) { return matrix->data(); }





template<typename T>
DatasetOverrides<T>::DatasetOverrides(const std::string& name,
                                      const std::string& full_group_path,
                                      std::shared_ptr<T> data_pointer,
                                      H5::H5File& file,
                                      int record_rate_hz,
                                      int buffer_length)
{
    dataset_name  = name;
    group_path    = full_group_path;
    logging_rate  = record_rate_hz;
    buffer_size   = buffer_size;
    hdf5_file_ptr = file;

    this->data_ptr(std::move(data_pointer)); // May not need this->
    datatype = deduceType();

    create_dataset(file);
}

// This is where I left off
template<typename T>
void DatasetOverrides<T>::create_dataset()
{
    try {
        auto shape = DataTraits<T>::shape();
        dataspace = H5::DataSpace(shape.size(), shape.data());

        H5::Group group = create_group_recursive(hdf5_file_ptr, group_path);
        dataset = group.createDataSet(dataset_name, datatype, dataspace);
    }
    catch (const H5::Exception& e) {
        std::cerr << "Error creating dataset: " << e.getDetailMsg() << std::endl;
    }
}

template<typename T>
void DatasetOverrides<T>::log_if_needed()
{
    try {
        const void* ptr = DataTraits<T>::data(data_ptr_.get());
        dataset.write(ptr, datatype);
    }
    catch (const H5::Exception& e) {
        std::cerr << "Error writing dataset: " << e.getDetailMsg() << std::endl;
    }
}

template<typename T>
void DatasetOverrides<T>::flush_buffer()
{
    // Placeholder – no buffer logic yet
}

#endif