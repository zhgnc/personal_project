#ifndef PORT_TYPE_NAME_HPP
#define PORT_TYPE_NAME_HPP

#include "../../math/math.hpp"

#include <cstdint>
#include <string>

// Maps C++ types to the readable name printed in wiring errors and the
// startup IO report, e.g. "quat<double>".
//
// C++ has no standard way to ask a type for a readable name: typeid(T).name()
// is implementation defined and returns a mangled string on gcc. Rather than
// decode that per compiler, the handful of types that can cross a port are
// simply named here. A type with no entry below fails to compile where the
// port is declared, which is also how the framework knows a type is not a
// valid port payload.
//
// Same shape as HDF5Type in data_logging/cpp_to_hdf5_type_mapping.hpp

template<typename T>
struct PortTypeName {
    static std::string get() {
        // This will fail and force the compiler to display the type T
        static_assert(sizeof(T) == 0, "PortTypeName not defined for this C++ type");
        return "";
    }
};

//***********//
// Integers //
//*********//
template<> struct PortTypeName<int8_t>   { static std::string get() { return "int8_t";   } };
template<> struct PortTypeName<uint8_t>  { static std::string get() { return "uint8_t";  } };
template<> struct PortTypeName<int16_t>  { static std::string get() { return "int16_t";  } };
template<> struct PortTypeName<uint16_t> { static std::string get() { return "uint16_t"; } };
template<> struct PortTypeName<int32_t>  { static std::string get() { return "int32_t";  } };
template<> struct PortTypeName<uint32_t> { static std::string get() { return "uint32_t"; } };
template<> struct PortTypeName<int64_t>  { static std::string get() { return "int64_t";  } };
template<> struct PortTypeName<uint64_t> { static std::string get() { return "uint64_t"; } };

//*******************//
// Floats & Doubles //
//*****************//
template<> struct PortTypeName<float>       { static std::string get() { return "float";       } };
template<> struct PortTypeName<double>      { static std::string get() { return "double";      } };
template<> struct PortTypeName<long double> { static std::string get() { return "long double"; } };

//**************//
// Other Types //
//************//
template<> struct PortTypeName<bool> { static std::string get() { return "bool"; } };
template<> struct PortTypeName<char> { static std::string get() { return "char"; } };

///////////////////
// Custom Types //
/////////////////

// Partial specializations so every size of the math types is covered without
// listing them one at a time

template<typename T, size_t N>
struct PortTypeName<vector<T, N>> {
    static std::string get() { return "vector<" + PortTypeName<T>::get() + ", " + std::to_string(N) + ">"; }
};

template<typename T, size_t R, size_t C>
struct PortTypeName<matrix<T, R, C>> {
    static std::string get() {
        return "matrix<" + PortTypeName<T>::get() + ", " + std::to_string(R) + ", " + std::to_string(C) + ">";
    }
};

template<typename T>
struct PortTypeName<quat<T>> {
    static std::string get() { return "quat<" + PortTypeName<T>::get() + ">"; }
};

template<typename T>
struct PortTypeName<rot_vec<T>> {
    static std::string get() { return "rot_vec<" + PortTypeName<T>::get() + ">"; }
};

#endif
