#include "external\hdf5\include\H5Cpp.h"
#include <cstdint>

// Maps C++ types to HDF5 library types when adding datasets 
template<typename T>
hid_t getHDF5Type();

//***********//
// Integers //
//*********//
template<>
hid_t getHDF5Type<int>() { return H5T_NATIVE_INT; }

template<>
hid_t getHDF5Type<unsigned int>() { return H5T_NATIVE_UINT; }

template<>
hid_t getHDF5Type<int8_t>() { return H5T_NATIVE_INT8; }

template<>
hid_t getHDF5Type<uint8_t>() { return H5T_NATIVE_UINT8; }

template<>
hid_t getHDF5Type<int16_t>() { return H5T_NATIVE_INT16; }

template<>
hid_t getHDF5Type<uint16_t>() { return H5T_NATIVE_UINT16; }

template<>
hid_t getHDF5Type<int32_t>() { return H5T_NATIVE_INT32; }

template<>
hid_t getHDF5Type<uint32_t>() { return H5T_NATIVE_UINT32; }

template<>
hid_t getHDF5Type<int64_t>() { return H5T_NATIVE_INT64; }

template<>
hid_t getHDF5Type<uint64_t>() { return H5T_NATIVE_UINT64; }

//*******************//
// Floats & Doubles //
//*****************//

template<>
hid_t getHDF5Type<float>() { return H5T_NATIVE_FLOAT; }

template<>
hid_t getHDF5Type<double>() { return H5T_NATIVE_DOUBLE; }

template<>
hid_t getHDF5Type<long double>() { return H5T_NATIVE_LDOUBLE; }

//**************//
// Other Types //
//************//

template<>
hid_t getHDF5Type<bool>() { return H5T_NATIVE_HBOOL; }

template<>
hid_t getHDF5Type<char>() { return 	H5T_NATIVE_CHAR; }

template<>
hid_t getHDF5Type<std::string>() {
    // H5::StrType is a HDF5 C++ RAII wrapper class to represent string datatypes
    
    // The constructor arguments for str_type are:
    // - H5::PredType::C_S1: specifies that the base character of the string as an 8-bit char
    // - H5T_VARIABLE: This constant indicates that the string is variable-length
    
    H5::StrType str_type(H5::PredType::C_S1, H5T_VARIABLE);
    return str_type.getId(); // getID() retuns the HDF5 string dataype (hid_t) 
}