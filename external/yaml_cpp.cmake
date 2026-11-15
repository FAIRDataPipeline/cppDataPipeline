set(YAML_CPP_BUILD_TESTS OFF CACHE INTERNAL "Build yaml-cpp tests")
if(BUILD_SHARED_LIBS)
    set(YAML_BUILD_SHARED_LIBS ON CACHE INTERNAL "Build .so for yaml-cpp")
endif()
set(YAML_CPP_INSTALL ON CACHE INTERNAL "Include export targets for installation")

fdpapi_add_external(
    "YAMLCPP"
    REPO "https://github.com/jbeder/yaml-cpp.git"
    TAG "yaml-cpp-0.9.0"
    PKG_NAME "yaml-cpp"
)
