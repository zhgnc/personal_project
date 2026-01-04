#include "../../src/data_logging/cpp_to_hdf5_type_mapping.hpp"

std::string hdf5_to_cpp_type_mapping(const H5::DataType& hdf5_type) {    
    struct TypeMapEntry { H5::PredType key; const char* value; };

    static const TypeMapEntry type_map[] = {
        { H5::PredType::NATIVE_INT8,    "int8_t" },
        { H5::PredType::NATIVE_UINT8,   "uint8_t" },
        { H5::PredType::NATIVE_INT16,   "int16_t" },
        { H5::PredType::NATIVE_UINT16,  "uint16_t" },
        { H5::PredType::NATIVE_INT32,   "int32_t" },
        { H5::PredType::NATIVE_UINT32,  "uint32_t" },
        { H5::PredType::NATIVE_INT64,   "int64_t" },
        { H5::PredType::NATIVE_UINT64,  "uint64_t" },
        { H5::PredType::NATIVE_FLOAT,   "float" },
        { H5::PredType::NATIVE_DOUBLE,  "double" },
        { H5::PredType::NATIVE_LDOUBLE, "long double" },
        { H5::PredType::NATIVE_HBOOL,   "bool" },
        { H5::PredType::NATIVE_CHAR,    "char" }
    };

    for (const TypeMapEntry& entry : type_map) {
        if (hdf5_type == entry.key) return entry.value;
    }

    switch (hdf5_type.getClass()) {
        case H5T_STRING: {
            const H5::StrType* string_type = &static_cast<const H5::StrType&>(hdf5_type);
            
            if (string_type != nullptr) {
                if (string_type->isVariableStr()) {
                    return "std::string";
                } else {
                    return "char[256]";
                }
            }

            break;
        }
        case H5T_COMPOUND: return "struct";
        case H5T_ARRAY:    return "array";
        default:           break;
    }

    return "unknown";
}