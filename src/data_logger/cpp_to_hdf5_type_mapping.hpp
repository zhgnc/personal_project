#ifndef CPP_TO_HDF5_TYPE_MAPPING_HPP
#define CPP_TO_HDF5_TYPE_MAPPING_HPP

#include "../../external/hdf5/include/H5Cpp.h"

#include <cstdint>

// Maps C++ types to HDF5 library types when adding datasets 
template<typename T>
H5::DataType getHDF5Type();

//***********//
// Integers //
//*********//
template<>
inline H5::DataType getHDF5Type<int8_t>() { return H5::PredType::NATIVE_INT8; }

template<>
inline H5::DataType getHDF5Type<uint8_t>() { return H5::PredType::NATIVE_UINT8; }

template<>
inline H5::DataType getHDF5Type<int16_t>() { return H5::PredType::NATIVE_INT16; }

template<>
inline H5::DataType getHDF5Type<uint16_t>() { return H5::PredType::NATIVE_UINT16; }

template<>
inline H5::DataType getHDF5Type<int32_t>() { return H5::PredType::NATIVE_INT32; }

template<>
inline H5::DataType getHDF5Type<uint32_t>() { return H5::PredType::NATIVE_UINT32; }

template<>
inline H5::DataType getHDF5Type<int64_t>() { return H5::PredType::NATIVE_INT64; }

template<>
inline H5::DataType getHDF5Type<uint64_t>() { return H5::PredType::NATIVE_UINT64; }

//*******************//
// Floats & Doubles //
//*****************//

template<>
inline H5::DataType getHDF5Type<float>() { return H5::PredType::NATIVE_FLOAT; }

template<>
inline H5::DataType getHDF5Type<double>() { return H5::PredType::NATIVE_DOUBLE; }

template<>
inline H5::DataType getHDF5Type<long double>() { return H5::PredType::NATIVE_LDOUBLE; }

//**************//
// Other Types //
//************//

template<>
inline H5::DataType getHDF5Type<bool>() { return H5::PredType::NATIVE_HBOOL; }

template<>
inline H5::DataType getHDF5Type<char>() { return H5::PredType::NATIVE_CHAR; }

template<>
inline H5::DataType getHDF5Type<std::string>() {
    // H5::StrType is a HDF5 C++ RAII wrapper class to represent string datatypes
    
    // The constructor arguments for str_type are:
    // - H5::PredType::C_S1: specifies that the base character of the string as an 8-bit char
    // - H5T_VARIABLE: This constant indicates that the string is variable-length
    
    H5::StrType str_type(H5::PredType::C_S1, H5T_VARIABLE);
    return str_type;
}

#endif 