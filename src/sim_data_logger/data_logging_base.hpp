#ifndef DATA_LOGGING_BASE_HPP
#define DATA_LOGGING_BASE_HPP

#include "../../external/hdf5/include/H5Cpp.h"

#include <string>

class DataLoggingBase {
public:
    virtual void create_dataset() = 0;
    virtual void log_if_needed() = 0;
    virtual void flush_buffer()  = 0;
    
    virtual ~DataLoggingBase() = default;

    std::string dataset_name;
    std::string group_path;

    int buffer_size;
    int logging_rate;

    H5::Group group;                       
    H5::DataSet dataset;                   
    H5::DataSpace dataspace;               
    H5::DataType datatype;                 
    H5::DSetCreatPropList property_list;
};

#endif