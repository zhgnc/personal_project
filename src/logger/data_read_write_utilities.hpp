#ifndef DATA_READ_WRITE_UTILITIES_HPP
#define DATA_READ_WRITE_UTILITIES_HPP

#include "../../src/logger/hdf5_logger.hpp"
#include "../../external/hdf5/include/H5Cpp.h"

template<typename T, std::size_t num_dimensions>
void HDF5Logger::write_data(const std::string& dataset_path, 
                            const T* data_pointer, 
                            const std::array<hsize_t, num_dimensions>& offsets, 
                            const std::array<hsize_t, num_dimensions>& data_shape) 
{
    desired_read_write_path = dataset_path;

    if (is_file_open() == false) {
        open_file();
    }

    H5::DataSet dataset = hdf5_file->openDataSet(dataset_path);
    HDF5Logger::check_desired_data_size(offsets, data_shape, dataset);

    H5::DataSpace where_to_write_data = dataset.getSpace();
    where_to_write_data.selectHyperslab(H5S_SELECT_SET, data_shape.data(), offsets.data());

    H5::DataSpace shape_of_data(num_dimensions, data_shape.data());

    dataset.write(data_pointer, getHDF5Type<T>(), shape_of_data, where_to_write_data);
    // Need to check that the data size can fit into desired spot, data types match, 
    // offset is within data size, and offset + data dimensions does not exceed dataset size
    
    // Want to add a write data function that just overwrites all data and only takes first 
    // two inputs from above. Also should add a function called write data point that writes 
    // a single datapoint to a location a dataset

};

template<typename T, std::size_t num_dimensions>
void HDF5Logger::read_data(const std::string& dataset_path, 
                           T* data_pointer, 
                           const std::array<hsize_t, num_dimensions>& offsets, 
                           const std::array<hsize_t, num_dimensions>& data_shape) 
{

    desired_read_write_path = dataset_path;

    if (is_file_open() == false) {
        open_file();
    }

    H5::DataSet dataset = hdf5_file->openDataSet(dataset_path);
    HDF5Logger::check_desired_data_size(offsets, data_shape, dataset);

    H5::DataSpace where_to_read_data = dataset.getSpace();
    where_to_read_data.selectHyperslab(H5S_SELECT_SET, data_shape.data(), offsets.data());

    H5::DataSpace shape_of_data(num_dimensions, data_shape.data());

    dataset.read(data_pointer, getHDF5Type<T>(), shape_of_data, where_to_read_data);

    // Need to add ability to read all data from a dataset and put in checks like mentioned above
};

template<std::size_t num_dimensions>
void HDF5Logger::check_desired_data_size(const std::array<hsize_t, num_dimensions>& offsets, 
                                         const std::array<hsize_t, num_dimensions>& data_shape, 
                                         const H5::DataSet& dataset) 
{
    std::ostringstream error_message;
    bool has_error = false;
    std::vector<hsize_t> dataset_dimensions = HDF5Logger::get_dataset_dimensions(dataset);
    
    for (std::size_t i = 0; i < num_dimensions; i++) {
        if (offsets[i] + data_shape[i] > dataset_dimensions[i]) {
            has_error = true;
            error_message << "Dataset '" << desired_read_write_path << "' dimension " << i 
                      << " exceeds dataset size: " << "offset (" << offsets[i] << ") + data_shape (" 
                      << data_shape[i] << ") > dataset_dim (" << dataset_dimensions[i] << ")\n";
        }
        // Do not need to check that the offsets or datashapes are positive because they unsigned ints
    }

    if (has_error) {
        throw std::runtime_error(error_message.str());
    }
};

#endif