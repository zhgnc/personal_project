#ifndef DATA_READ_WRITE_UTILITIES_HPP
#define DATA_READ_WRITE_UTILITIES_HPP

#include "../../src/data_logger/hdf5_logger.hpp"
#include "../../external/hdf5/include/H5Cpp.h"

template<typename T>
void HDF5Logger::write_data(const std::string& path_to_data_set, 
                            const T* data_pointer, 
                            const std::initializer_list<hsize_t>& offsets, 
                            const std::initializer_list<hsize_t>& data_shape) 
{
    dataset_path = path_to_data_set;
    HDF5Logger::verify_file_exists();

    if (is_file_open() == false) {
        open_file();
    }

    std::vector<hsize_t> offset_vector(offsets);
    std::vector<hsize_t> data_shape_vector(data_shape);
    std::size_t num_dimensions = offsets.size();

    H5::DataSet dataset = hdf5_file->openDataSet(dataset_path);
    HDF5Logger::check_desired_data_size(offset_vector, data_shape_vector, dataset);

    H5::DataSpace where_to_write_data = dataset.getSpace();
    where_to_write_data.selectHyperslab(H5S_SELECT_SET, data_shape_vector.data(), offset_vector.data());

    H5::DataSpace shape_of_data(num_dimensions, data_shape_vector.data());

    dataset.write(data_pointer, getHDF5Type<T>(), shape_of_data, where_to_write_data);
    // Need to check that the data size can fit into desired spot, data types match, 
    // offset is within data size, and offset + data dimensions does not exceed dataset size
    
    // Want to add a write data function that just overwrites all data and only takes first 
    // two inputs from above. Also should add a function called write data point that writes 
    // a single datapoint to a location a dataset

};

template<typename T>
void HDF5Logger::append_data(const std::string& path_to_data_set,
                             const std::initializer_list<hsize_t>& data_shape_to_append,
                             const T* data_pointer) 
{
    dataset_path = path_to_data_set;
    HDF5Logger::verify_file_exists();

    if (is_file_open() == false) {
        open_file();
    }

    std::vector<hsize_t> data_shape_vec(data_shape_to_append);
    std::size_t num_dimensions = data_shape_to_append.size();

    H5::DataSet dataset = hdf5_file->openDataSet(dataset_path);
    HDF5Logger::check_desired_data_size(data_shape_vec, dataset);

    std::vector<hsize_t> dataset_dimensions = HDF5Logger::get_dataset_dimensions(dataset);
    dataset_dimensions[0] += 1;

    dataset.extend(dataset_dimensions.data());
    H5::DataSpace where_to_write_data = dataset.getSpace();

    std::vector<hsize_t> offsets(num_dimensions + 1);
    offsets[0] = dataset_dimensions[0] - 1; // dataset_dimensions is 1 past the last element (due to non-zero based indexing)

    std::vector<hsize_t> expanded_data_shape(num_dimensions + 1);
    expanded_data_shape[0] = 1;  // appending one slice
    for (size_t i = 0; i < num_dimensions; i++) {
        expanded_data_shape[i+1] = data_shape_vec[i];
    }

    where_to_write_data.selectHyperslab(H5S_SELECT_SET, data_shape_vec.data(), offsets.data());
    H5::DataSpace data_write_dimensions(num_dimensions + 1, expanded_data_shape.data());

    dataset.write(data_pointer, getHDF5Type<T>(), data_write_dimensions, where_to_write_data);
};

template<typename T>
void HDF5Logger::read_data(const std::string& path_to_data_set, 
                           T* data_pointer, 
                           const std::initializer_list<hsize_t>& offsets, 
                           const std::initializer_list<hsize_t>& data_shape) 
{
    dataset_path = path_to_data_set;
    HDF5Logger::verify_file_exists();

    if (is_file_open() == false) {
        open_file();
    }

    std::vector<hsize_t> offset_vector(offsets);
    std::vector<hsize_t> data_shape_vector(data_shape);
    std::size_t num_dimensions = offsets.size();

    H5::DataSet dataset = hdf5_file->openDataSet(dataset_path);
    HDF5Logger::check_desired_data_size(offset_vector, data_shape_vector, dataset);

    H5::DataSpace where_to_read_data = dataset.getSpace();
    where_to_read_data.selectHyperslab(H5S_SELECT_SET, data_shape_vector.data(), offset_vector.data());

    H5::DataSpace shape_of_data(num_dimensions, data_shape_vector.data());

    dataset.read(data_pointer, getHDF5Type<T>(), shape_of_data, where_to_read_data);

    // Need to add ability to read all data from a dataset and put in checks like mentioned above
};

inline void HDF5Logger::check_desired_data_size(const std::vector<hsize_t>& offsets, 
                                         const std::vector<hsize_t>& data_shape, 
                                         const H5::DataSet& dataset) 
{
    std::ostringstream error_message;
    bool has_error = false;
    std::vector<hsize_t> dataset_dimensions = HDF5Logger::get_dataset_dimensions(dataset);

    std::size_t num_dimensions = offsets.size();
    
    for (std::size_t i = 0; i < num_dimensions; i++) {
        if (offsets[i] + data_shape[i] > dataset_dimensions[i]) {
            has_error = true;
            error_message << "Dataset '" << dataset_path << "' dimension " << i 
                      << " is less than data shape + offset sizes: " << "offset (" << offsets[i] << ") + data_shape (" 
                      << data_shape[i] << ") > dataset_dim (" << dataset_dimensions[i] << ")\n";
        }
        // Do not need to check that the offset or datashape sizes are positive because they unsigned ints
    }

    if (has_error) {
        throw std::runtime_error(error_message.str());
    }
};

inline void HDF5Logger::check_desired_data_size(const std::vector<hsize_t>& data_shape, const H5::DataSet& dataset) 
{
    std::ostringstream error_message;
    bool has_error = false;
    std::vector<hsize_t> dataset_dimensions = HDF5Logger::get_dataset_dimensions(dataset);
    hsize_t one_element_slice = 1;

    std::size_t num_dimensions = data_shape.size();
    
    if (data_shape[0] != one_element_slice) {
        has_error = true;
        error_message << "Datashape dimension 0 should be 1 when appending to this dataset ('" 
                      << full_file_path << "') but is equal to " << data_shape[0] << "\n";
    }
    
    for (std::size_t i = 1; i < num_dimensions; i++) {
        if (data_shape[i] != dataset_dimensions[i]) {
            has_error = true;
            error_message << "Dataset '" << full_file_path << "' dimension " << i 
                      << " should equal data shape size: " << "data_shape (" << data_shape[i] << 
                      ") != dataset_dim (" << dataset_dimensions[i] << ")\n";
        }
    }

    // Do not need to check that the datashape sizes are positive because they unsigned ints

    if (has_error) {
        throw std::runtime_error(error_message.str());
    }
};

#endif