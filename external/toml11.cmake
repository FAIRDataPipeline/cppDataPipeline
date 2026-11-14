# toml11 >= 3.8 requires CMAKE_CXX_STANDARD to be set before it is configured
# (older versions set it to 11 themselves).
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

fdpapi_add_external(
    "TOML11"
    URL "https://github.com/ToruNiina/toml11/archive/refs/tags/v3.8.1.zip" 
    PKG_NAME "toml11"
)
