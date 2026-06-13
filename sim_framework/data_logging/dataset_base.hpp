#ifndef DATA_LOGGING_BASE_HPP
#define DATA_LOGGING_BASE_HPP

#include "../../external/hdf5/include/H5Cpp.h"

#include <memory>
#include <string>

class DatasetBase {
public:
    virtual void create_dataset() = 0;
    virtual void log_if_needed(const uint64_t &sim_time_usec) = 0;
    virtual void flush_buffer()  = 0;
    
    virtual ~DatasetBase() = default;

    std::string dataset_name;
    std::string group_path;

    double logging_rate;
    uint64_t logging_dt_usec;
    double sec2usec = 1e6;

    std::shared_ptr<H5::H5File> hdf5_file_ptr;

    std::size_t buffer_index; 

    H5::Group group;                       
    H5::DataSet dataset;                   
    H5::DataSpace dataspace;               
    H5::DSetCreatPropList property_list;
    H5::DataType hdf5_data_type;
    H5::DataSpace file_space;
    H5::DataSpace memory_space;
};

#endif