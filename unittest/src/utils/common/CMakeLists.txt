add_executable(testcommon
        StringTokenizerTest.cpp
        FileHelpersTest.cpp
        StringUtilsTest.cpp
        RGBColorTest.cpp
        ValueTimeLineTest.cpp
        )
setTestProperties(testcommon utils_common utils_iodevices)
