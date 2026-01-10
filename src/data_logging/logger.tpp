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
    
    std::string full_path_to_dataset = full_group_path + "/" + dataset_name;
    write_attribute(full_path_to_dataset, "Logging Rate Hz", record_rate_hz);
};

template<typename T>
void Logger::write_attribute(const std::string& object_path,
                             const std::string& attribute_name,
                             const T& value)
{
    if (file_is_open == false) {
        open_file();
        file_is_open = true; 
    }

    
    // Verify the group or dataset exists
    if (hdf5_file_ptr->nameExists(object_path) == false) {
        throw std::runtime_error("[logger.tpp] HDF5 object (group or dataset) does not exist: " + object_path);
    }

    
    // Get object info and open correct object type
    H5O_info2_t object_info{};
    unsigned fields = H5O_INFO_BASIC;  // only basic info (type, etc.)
    H5Oget_info_by_name3(hdf5_file_ptr->getId(), object_path.c_str(), &object_info, fields, H5P_DEFAULT);

    
    
    // H5::Group and H5::DataSet are different types so each object must be created 
    // in each conditional statement separately. The helper function is created to 
    // not have to duplicate code in both branches of the conditional depending on 
    // if the H5 object was a group or a dataset 
    if (object_info.type == H5O_TYPE_GROUP) {
        H5::Group group = hdf5_file_ptr->openGroup(object_path);
        write_attribute_to_generic_object(group, attribute_name, value);

    } else if (object_info.type == H5O_TYPE_DATASET) {
        H5::DataSet dataset = hdf5_file_ptr->openDataSet(object_path);
        write_attribute_to_generic_object(dataset, attribute_name, value);

    } else {
        throw std::runtime_error("Unsupported HDF5 object type: " + object_path);
    }
};

template<typename H5ObjType, typename T>
void Logger::write_attribute_to_generic_object(H5ObjType& obj, const std::string& attribute_name, const T& value) { 
    // If the attribute already exists, HDF5 will throw an exception
    if (obj.attrExists(attribute_name)) {
        obj.removeAttr(attribute_name);
    }

    H5::DataType datatype = HDF5Type<T>::get();
    H5::DataSpace dataspace(H5S_SCALAR);
    H5::Attribute attr = obj.createAttribute(attribute_name, datatype, dataspace);
    
    // Note, HDF5 writes std::strings by value to attributes and by reference for numerical types.
    // Without constexpr the compiler would try to compile both branches for all data types, 
    // but since hdf5 cannot write a string by reference this would cause the compiler to fail 
    if constexpr (std::is_same_v<T,std::string>)
        attr.write(datatype, value);
    else
        attr.write(datatype, &value);
}