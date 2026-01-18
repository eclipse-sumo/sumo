add_executable(testcommon
        FileHelpersTest.cpp
        RandHelperTest.cpp
        RGBColorTest.cpp
        StringTokenizerTest.cpp
        StringUtilsTest.cpp
        ValueTimeLineTest.cpp
        )
# the commonlibs get linked anyway, no need to repeat them here
setTestProperties(testcommon)
