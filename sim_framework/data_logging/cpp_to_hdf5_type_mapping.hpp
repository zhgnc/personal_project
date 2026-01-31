#ifndef CPP_TO_HDF5_TYPE_MAPPING_HPP
#define CPP_TO_HDF5_TYPE_MAPPING_HPP

#include "../../external/hdf5/include/H5Cpp.h"
#include "../../math//math.hpp"

#include <cstdint>

// Maps C++ types to HDF5 library types when adding datasets 
template<typename T>
struct HDF5Type;

//***********//
// Integers //
//*********//
template<> struct HDF5Type<int8_t>   { static H5::DataType get() { return H5::PredType::NATIVE_INT8;   } };
template<> struct HDF5Type<uint8_t>  { static H5::DataType get() { return H5::PredType::NATIVE_UINT8;  } };
template<> struct HDF5Type<int16_t>  { static H5::DataType get() { return H5::PredType::NATIVE_INT16;  } };
template<> struct HDF5Type<uint16_t> { static H5::DataType get() { return H5::PredType::NATIVE_UINT16; } };
template<> struct HDF5Type<int32_t>  { static H5::DataType get() { return H5::PredType::NATIVE_INT32;  } };
template<> struct HDF5Type<uint32_t> { static H5::DataType get() { return H5::PredType::NATIVE_UINT32; } };
template<> struct HDF5Type<int64_t>  { static H5::DataType get() { return H5::PredType::NATIVE_INT64;  } };
template<> struct HDF5Type<uint64_t> { static H5::DataType get() { return H5::PredType::NATIVE_UINT64; } };

//*******************//
// Floats & Doubles //
//*****************//

template<> struct HDF5Type<float>       { static H5::DataType get() { return H5::PredType::NATIVE_FLOAT;   } };
template<> struct HDF5Type<double>      { static H5::DataType get() { return H5::PredType::NATIVE_DOUBLE;  } };
template<> struct HDF5Type<long double> { static H5::DataType get() { return H5::PredType::NATIVE_LDOUBLE; } };

//**************//
// Other Types //
//************//

template<> struct HDF5Type<bool> { static H5::DataType get() { return H5::PredType::NATIVE_HBOOL; } };
template<> struct HDF5Type<char> { static H5::DataType get() { return H5::PredType::NATIVE_CHAR;  } };

template<> struct HDF5Type<std::string> {
    // H5::StrType is a HDF5 C++ RAII wrapper class to represent string datatypes
    
    // The constructor arguments for str_type are:
    // - H5::PredType::C_S1: specifies that the base character of the string as an 8-bit char
    // - H5T_VARIABLE: This constant indicates that the string is variable-length
    static H5::DataType get() {    
        H5::StrType str_type(H5::PredType::C_S1, H5T_VARIABLE);
        return str_type;
    }
};

template<> struct HDF5Type<char*>       { static H5::DataType get() { return HDF5Type<std::string>::get(); }};
template<> struct HDF5Type<const char*> { static H5::DataType get() { return HDF5Type<std::string>::get(); }};

///////////////////
// Custom Types //
/////////////////

// C++ does not allows partial template specialization for functions so a class must be used to return 
// the hdf5 type for the math types in 'math/' with more than 1 template parameter

template<typename T>
struct HDF5Type {
    static H5::DataType get() {
        // This will fail and force the compiler to display the type T
        static_assert(sizeof(T) == 0, "HDF5Type not defined for this C++ type");
        return H5::DataType{};
    }
};

template<typename T, size_t N> 
struct HDF5Type<vector<T, N>> { 
    static H5::DataType get() {
        return HDF5Type<T>::get();
    }
};

template<typename T, size_t R, size_t C> 
struct HDF5Type<matrix<T, R, C>> { 
    static H5::DataType get() {
        return HDF5Type<T>::get();
    }
};

#endif 