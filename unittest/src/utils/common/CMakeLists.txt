add_executable(testcommon
        FileHelpersTest.cpp
        RandHelperTest.cpp
        RGBColorTest.cpp
        StringTokenizerTest.cpp
        StringUtilsTest.cpp
        ValueTimeLineTest.cpp
        )
setTestProperties(testcommon utils_common utils_iodevices)
