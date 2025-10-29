#ifndef DATA_LOGGING_BASE_HPP
#define DATA_LOGGING_BASE_HPP

#include "../../external/hdf5/include/H5Cpp.h"

#include <string>

class DatasetBase {
public:
    virtual void create_dataset() = 0;
    virtual void log_if_needed() = 0;
    virtual void flush_buffer()  = 0;
    
    virtual ~DatasetBase() = default;

    std::string dataset_name;
    std::string group_path;

    int buffer_size;
    int logging_rate;
    uint32_t logging_dt_usec;
    double sec2usec = 1e6;

    std::shared_ptr<H5::H5File> hdf5_file_ptr;

    H5::Group group;                       
    H5::DataSet dataset;                   
    H5::DataSpace dataspace;               
    H5::DataType datatype;                 
    H5::DSetCreatPropList property_list;
    H5::DataType hdf5_data_type;
};

#endif