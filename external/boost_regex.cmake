MESSAGE( STATUS "[Boost Regex]" )

set(BRX_URL "https://github.com/boostorg/regex/archive/refs/tags/boost-1.79.0.zip")

MESSAGE( STATUS "\tBoost Regex Will be installed." )
MESSAGE( STATUS "\tURL: ${BRX_URL}" )

set(BOOST_REGEX_STANDALONE ON)

include(FetchContent)
FetchContent_Declare(
    BRX
    URL ${BRX_URL}
)
FetchContent_MakeAvailable(BRX)