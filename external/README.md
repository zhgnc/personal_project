# External Included Libraries
This project currently uses the following libraries. The libraries other the Eigen, since it is
header only, have been statically linked to avoid runtime dependenies and recompilation during
each build. 

- Eigen 
    - Templated matrix library for unit testing the math library in `src/math_utilties`
    - Version: 5.0.0
    - link: https://eigen.tuxfamily.org/index.php?title=Main_Page
- Google Test
    - Unit test framework
    - Version: 1.17.0
    - link: https://github.com/google/googletest
- HDF5
    - Official HDF5 C library with access to both the core HDF5 C API (`libhdf5.a`) and the high-level HDf5 API (`libhdf5_hl.a`)
    - Version: develop cloned on 10/9/25
    - links: https://github.com/HDFGroup/hdf5 and https://docs.hdfgroup.org/archive/support/HDF5/doc1.8/cpplus_RM/index.html
- yaml-cpp
    - Yaml parser for C++
    - Version: develop cloned on 10/9/25 
    - link: https://github.com/jbeder/yaml-cpp