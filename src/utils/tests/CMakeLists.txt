set(utils_tests_STAT_SRCS
    InternalTest.cpp
    InternalTest.h
    InternalTestStep.cpp
    InternalTestStep.h
)

add_library(utils_tests STATIC ${utils_tests_STAT_SRCS})
set_property(TARGET utils_tests PROPERTY PROJECT_LABEL "z_utils_tests")

add_custom_command(OUTPUT attributesEnum.txt contextualMenuOperations.txt viewPositions.txt
                   COMMAND ${PYTHON_EXECUTABLE} ${SUMO_TOOLS_DIR}/build_config/internaltestdata.py ${SUMO_TOOLS_DIR} ${CMAKE_CURRENT_BINARY_DIR}

                  )
add_custom_target(generate-internaltestdata-h
                  DEPENDS attributesEnum.txt contextualMenuOperations.txt attributesEnum.txt
                 )
add_dependencies(utils_tests generate-internaltestdata-h)
set_property(TARGET generate-internaltestdata-h PROPERTY FOLDER "CMake")
