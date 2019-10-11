cmake_minimum_required(VERSION 3.10.2)
project(dic_project CXX)

set(targetPlatform debian CACHE STRING "target platform")
set_property(CACHE targetPlatform PROPERTY STRINGS debian redhat freeBSD windows)

if(targetPlatform STREQUAL windows)
    message("cross compilling for windows")
    # cmake -DCMAKE_TOOLCHAIN_FILE=myToolchain.cmake path/to/source
    #set(CMAKE_CXX_COMPILER g++)
    #set(extraOpts "-Wall -Wextra")
    #set(CMAKE_C_FLAGS_DEBUG_INIT ${extraOpts})
    #set(CMAKE_CXX_FLAGS_DEBUG_INIT ${extraOpts})

    #set(CMAKE_SYSTEM_NAME Linux)
    #set(CMAKE_SYSTEM_PROCESSOR ARM)

    #install "g++-mingw-w64-x86-64" in ubuntu
    set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
    set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

endif()

include (GNUInstallDirs)
message (BINDIR)
message (LIBDIR)
message (DATADIR)
message (DATAROOTDIR)

add_subdirectory(src)
