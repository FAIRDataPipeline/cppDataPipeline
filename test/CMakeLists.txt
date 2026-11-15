find_package(Threads REQUIRED)

# Include CTest for testing
include(CTest)

# Enable Testing to ensure testing works
enable_testing()

# Set the TESTDIR in the cmake CXX flags
set(CMAKE_CXX_FLAGS "-DTESTDIR=\\\"${CMAKE_CURRENT_SOURCE_DIR}\\\" ${CMAKE_CXX_FLAGS}")

# Set Test Name
set(TEST_NAME fdpapi-tests)

# Assume built-in pthreads on MacOS
if(APPLE)
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
endif()

# Use win32 threads on windows, disabling pthreads in gtest
if(WIN32)
    option(CMAKE_USE_WIN32_THREADS_INIT "using WIN32 threads" ON)
    option(gtest_disable_pthreads "Disable uses of pthreads in gtest." ON)
endif()

# Get google test suit
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Find all files matching test naming (test_<name>.cxx)
file(GLOB_RECURSE src_unit CONFIGURE_DEPENDS "test_*.cxx")

# Add the test files to the executable
add_executable(fdpapi-tests ${src_unit})

# Add compile options to tests
add_compile_options(fdpapi-tests ${BUILD_TYPE_COMPILE_FLAGS})

# Add includes to tests
target_include_directories(fdpapi-tests PUBLIC ${pybind11_INCLUDE_DIRS})
target_include_directories(fdpapi-tests PUBLIC ${PYTHON_INCLUDE_DIRS})

message(STATUS "----- Configuring Test Build -----")
message(STATUS "[Libraries]:")
message(STATUS "\tAPI C++ BINDINGS: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "\tGTEST: ${GTEST_INCLUDE_DIRS}")
message(STATUS "\tGMOCK: ${GMOCK_INCLUDE_DIRS}")
message(STATUS "\tFLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "----------------------------------")

# Dependencies
target_link_libraries(fdpapi-tests PRIVATE fdpapi::fdpapi)
target_link_libraries(fdpapi-tests PRIVATE gtest gtest_main)

target_link_libraries(fdpapi-tests PRIVATE toml11::toml11)
target_link_libraries(fdpapi-tests PRIVATE digestpp::digestpp)
target_link_libraries(fdpapi-tests PRIVATE CURL::libcurl)
target_link_libraries(fdpapi-tests PRIVATE yaml-cpp)
if(BUILD_SHARED_LIBS)
    target_link_libraries(fdpapi-tests PRIVATE jsoncpp_lib)
else()
    target_link_libraries(fdpapi-tests PRIVATE jsoncpp_static)
endif()
target_link_libraries(fdpapi-tests PRIVATE ghcFilesystem::ghc_filesystem)

# Include GoogleTest Library
include(GoogleTest)
# Use GoogleTest Libary to Discover the tests
gtest_discover_tests(fdpapi-tests)

# Compile Code Coverage if Specified with Tests
if(FDPAPI_CODE_COVERAGE)
    if(CMAKE_COMPILER_IS_GNUCXX)
        LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules")
        include(CodeCoverage)
        append_coverage_compiler_flags()
        setup_target_for_coverage_gcovr_sonar(
            NAME coverage
            EXECUTABLE bin/fdpapi-tests
            DEPENDENCIES fdpapi-tests
            BASE_DIRECTORY ${PROJECT_SOURCE_DIR}
        )
    endif()
endif()
