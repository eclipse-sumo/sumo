# Compile the consumer against an installation without access to SUMO's build includes.
string(RANDOM LENGTH 12 ALPHABET "0123456789abcdef" test_id)
set(test_dir "${SUMO_BUILD_DIR}/libsumo-external-${test_id}")
set(install_dir "${test_dir}/install")
set(build_dir "${test_dir}/build")
set(config_args)
set(ctest_config_args)
set(generator_args --build-generator "${SUMO_GENERATOR}")
if (SUMO_CONFIG)
    list(APPEND config_args --config "${SUMO_CONFIG}")
    list(APPEND ctest_config_args -C "${SUMO_CONFIG}")
endif ()
if (SUMO_GENERATOR_PLATFORM)
    list(APPEND generator_args --build-generator-platform "${SUMO_GENERATOR_PLATFORM}")
endif ()
if (SUMO_GENERATOR_TOOLSET)
    list(APPEND generator_args --build-generator-toolset "${SUMO_GENERATOR_TOOLSET}")
endif ()
if (SUMO_MAKE_PROGRAM)
    list(APPEND generator_args --build-makeprogram "${SUMO_MAKE_PROGRAM}")
endif ()

unset(ENV{DESTDIR})
execute_process(COMMAND "${CMAKE_COMMAND}" --install "${SUMO_BUILD_DIR}" --prefix "${install_dir}"
    ${config_args} --component runtime COMMAND_ERROR_IS_FATAL ANY)
if (SUMO_INSTALL_DLLS)
    execute_process(COMMAND "${CMAKE_COMMAND}" --install "${SUMO_BUILD_DIR}" --prefix "${install_dir}"
        ${config_args} --component dll COMMAND_ERROR_IS_FATAL ANY)
endif ()

execute_process(COMMAND "${SUMO_CTEST_COMMAND}" --build-and-test "${CMAKE_CURRENT_LIST_DIR}" "${build_dir}"
    ${generator_args} ${ctest_config_args}
    --build-options "-DSUMO_DIR=${install_dir}/${SUMO_CMAKE_DIR}"
        "-DSUMO_INSTALL_BIN=${install_dir}/bin"
        "-DCMAKE_CXX_COMPILER=${SUMO_CXX_COMPILER}" "-DCMAKE_CXX_COMPILER_LAUNCHER="
    --test-command "${SUMO_CTEST_COMMAND}" ${ctest_config_args} --output-on-failure
    COMMAND_ERROR_IS_FATAL ANY)
file(REMOVE_RECURSE "${test_dir}")
