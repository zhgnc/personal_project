#include "logger.hpp"
#include "cpp_to_hdf5_type_mapping.hpp"

template<typename T>
void Logger::add_dataset(const std::string& dataset_name, 
                         const std::string& full_group_path, 
                         const T& data_reference, 
                         const double record_rate_hz) 
{
    verify_group_exists(full_group_path);

    // [](const T*) {} ensures shared_ptr does not delete the referenced object
    std::shared_ptr<const T> data_pointer = std::shared_ptr<const T>(&data_reference, [](const T*) {}); 

    std::unique_ptr<DatasetBase> dataset  = std::make_unique<DatasetOverrides<T, buffer_length_config>>(
            dataset_name, full_group_path, data_pointer, hdf5_file_ptr, record_rate_hz); 
    
    dataset->create_dataset();
    datasets.push_back(std::move(dataset));
};

template<typename T>
void Logger::write_attribute(const std::string& group_path,
                             const std::string& attribute_name,
                             const T& value)
{
    if (file_is_open == false) {
        open_file();
        file_is_open = true; 
    }

    verify_group_exists(group_path);
    H5::Group group = hdf5_file_ptr->openGroup(group_path);

    // If the attribute already exists, HDF5 will throw an exception
    if (group.attrExists(attribute_name)) {
        group.removeAttr(attribute_name);
    }

    H5::DataType datatype = HDF5Type<T>::get();
    H5::DataSpace attr_dataspace(H5S_SCALAR); // H5S_SCALAR stores a single value/variable
    H5::Attribute attr = group.createAttribute(attribute_name, datatype, attr_dataspace);

    // Note, HDF5 writes std::strings by value to attributes and by reference for numerical types.
    // Without constexpr the compiler would try to compile both branches for all data types, 
    // but since hdf5 cannot write a string by reference this would cause the compiler to fail 
    if constexpr (std::is_same_v<T, std::string>) {
        attr.write(datatype, value);  
    } else {
        attr.write(datatype, &value);
    }
};