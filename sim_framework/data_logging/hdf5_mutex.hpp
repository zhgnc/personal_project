#ifndef HDF5_MUTEX_HPP
#define HDF5_MUTEX_HPP

#include <mutex>

// The vendored HDF5 library (and its C++ wrapper) is built without thread-safety support
// (see external/hdf5/lib/libhdf5.settings: `Threadsafety: OFF`), so every call into the
// H5:: API must be serialized through this single process-wide lock, even though each
// sim thread owns its own Logger/H5::H5File. Shared by Logger and DatasetOverrides.
// A recursive_mutex is used because locked functions call each other on the same thread
// (e.g. Logger::close_file() -> DatasetOverrides::flush_buffer()).
//
// Functions that only touch per-Logger memory (dataset buffer copies, std::filesystem
// checks) intentionally do NOT take this lock so sim threads never stall on another
// thread's file I/O during their step loop.
inline std::recursive_mutex hdf5_mutex;

#endif
