set(JSONCPP_WITH_TESTS OFF CACHE INTERNAL "Don't build json-cpp tests")
fdpapi_add_external(
    "JSONCPP"
    URL "https://github.com/open-source-parsers/jsoncpp/archive/refs/tags/1.9.6.zip"
    PKG_NAME "jsoncpp"
)
