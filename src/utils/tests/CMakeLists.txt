set(utils_tests_STAT_SRCS
    InternalTest.cpp
    InternalTest.h
    InternalTestStep.cpp
    InternalTestStep.h
)

add_library(utils_tests STATIC ${utils_tests_STAT_SRCS})
set_property(TARGET utils_tests PROPERTY PROJECT_LABEL "z_utils_tests")

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/attributesEnum.txt
                          ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/contextualMenuOperations.txt
                          ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/viewPositions.txt
                   COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../tools/build_config/internaltestdata.py ${CMAKE_CURRENT_BINARY_DIR}
                  )
add_custom_target(generate-internaltestdata-h
                  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/attributesEnum.txt
                          ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/contextualMenuOperations.txt
                          ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/attributesEnum.txt
                 )
add_dependencies(utils_tests generate-internaltestdata-h)
set_property(TARGET generate-internaltestdata-h PROPERTY FOLDER "CMake")