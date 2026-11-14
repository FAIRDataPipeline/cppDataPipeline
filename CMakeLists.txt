cmake_minimum_required(VERSION 3.13)

# Define Project
project(fdpapi
    VERSION 0.2.8
    DESCRIPTION "C++ API for the FAIR Data Pipeline"
    HOMEPAGE_URL "https://github.com/FAIRDataPipeline/cppDataPipeline"
    LANGUAGES CXX C
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(FetchContent)

# json-cpp library forces BUILD_SHARED_LIBS to ON by default -- fixing that here.
option(BUILD_SHARED_LIBS "Build Static Libraries" OFF)

# Default Options for Tests, Code Coverage, installation
option(FDPAPI_BUILD_TESTS  "Build unit tests" OFF)
option(FDPAPI_CODE_COVERAGE "Run GCov and LCov code coverage tools" OFF)
option(FDPAPI_WITH_INSTALL "Allow project to be installable" ON)
option(FDPAPI_ALWAYS_FETCH "Don't use pre-installed dependencies, use FetchContent instead" OFF)
option(FDPAPI_NEVER_FETCH "Only use pre-installed dependencies, don't use FetchContent" OFF)
if(FDPAPI_ALWAYS_FETCH AND FDPAPI_NEVER_FETCH)
    message(FATAL_ERROR "FDPAPI_ALWAYS_FETCH and FDPAPI_NEVER_FETCH are mutually exclusive")
endif()

#  Disable Building in Debug as HD5 
get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(isMultiConfig)
    set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "" FORCE)
    message("Multi-Config Generator found, please ensure you are building with a realease configuration")
else()
    if(NOT CMAKE_BUILD_TYPE)
        message("Defaulting to release build.")
        set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
    endif()
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING "Choose the type of build")
    # set the valid options for cmake-gui drop-down list
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release")
endif()


# Set Output Directories to avoid issues on multi release compilers
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Set Module Path to include external directory
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/external")

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
	cmake_policy(SET CMP0135 NEW)
endif()

# Include macro for adding external packages
include(cmake_modules/fdpapi_add_external.cmake)

# Include external library files
include(external/jsoncpp.cmake)
include(external/curl.cmake)
include(external/yaml_cpp.cmake)
include(external/toml11.cmake)
include(external/ghc.cmake)
include(external/digestpp.cmake)

# Define and install library
add_subdirectory(./src)

# Compile Tests if specified
if(FDPAPI_BUILD_TESTS)
    add_subdirectory(test)
endif()
