SET(testcommon_STAT_SRCS
   StringTokenizerTest.cpp
   StringUtilsTest.cpp
   TplConvertTest.cpp
   RGBColorTest.cpp
   ValueTimeLineTest.cpp
   CommandMock.h
)

add_library(testcommon STATIC ${testcommon_STAT_SRCS})
