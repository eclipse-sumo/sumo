set(utils_tests_STAT_SRCS
    DialogType.h
    InternalTest.cpp
    InternalTest.h
    InternalTestStep.cpp
    InternalTestStep.h
)

add_library(utils_tests STATIC ${utils_tests_STAT_SRCS})
set_property(TARGET utils_tests PROPERTY PROJECT_LABEL "z_utils_tests")
