# utility function to install headers from a file list
function(install_headers _file_list)
    # install headers
    foreach(f ${_file_list})
        get_filename_component(_ext ${f} EXT)
        if(NOT _ext STREQUAL ".h")
            continue()
        endif()
        file(RELATIVE_PATH _sub_dir ${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR})
        install(FILES ${f}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sumo/${_sub_dir}
            COMPONENT Development)
    endforeach()
endfunction()

# utility function to add target to cmake export config
function(export_target target _file_list)
    # install headers
    install_headers("${_file_list}")
    # set package include dir
    target_include_directories(libsumocpp PUBLIC
        ${target_BUILD_INTERFACE_LIST}
        $<INSTALL_INTERFACE:include/sumo>
    )
    # add target to export package
    install( TARGETS libsumocpp EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION bin COMPONENT RuntimeLibraries
        LIBRARY DESTINATION lib COMPONENT RuntimeLibraries
        ARCHIVE DESTINATION lib COMPONENT Development)
endfunction()

set(netconvertlibs
        netwrite netimport netbuild foreign_eulerspiral ${GDAL_LIBRARY} netimport_vissim netimport_vissim_typeloader netimport_vissim_tempstructs ${commonlibs})

set(sumolibs
        traciserver netload microsim_cfmodels microsim_engine microsim_lcmodels microsim_devices microsim_trigger microsim_output microsim_transportables microsim_actions
        microsim_traffic_lights microsim mesosim ${commonvehiclelibs} ${PYTHON_LIBRARIES})

add_subdirectory(activitygen)
add_subdirectory(dfrouter)
add_subdirectory(duarouter)
if (FMI)
    add_subdirectory(fmi)
endif ()
add_subdirectory(foreign)
add_subdirectory(jtrrouter)
add_subdirectory(libsumo)
add_subdirectory(libtraci)
add_subdirectory(marouter)
add_subdirectory(mesosim)
add_subdirectory(microsim)
add_subdirectory(netbuild)
add_subdirectory(netgen)
add_subdirectory(netimport)
add_subdirectory(netload)
add_subdirectory(netwrite)
add_subdirectory(od)
if (OPENSCENEGRAPH_FOUND)
    add_subdirectory(osgview)
    set(osgviewlib osgview)
endif ()
add_subdirectory(polyconvert)
add_subdirectory(router)
add_subdirectory(tools)
add_subdirectory(traci-server)
add_subdirectory(traci_testclient)
add_subdirectory(utils)
if (FOX_FOUND)
    add_subdirectory(gui)
    add_subdirectory(guinetload)
    add_subdirectory(guisim)
    add_subdirectory(mesogui)
    add_subdirectory(netedit)
endif ()


add_executable(sumo sumo_main.cpp)
set_target_properties(sumo PROPERTIES OUTPUT_NAME sumo${BINARY_SUFFIX})
set_target_properties(sumo PROPERTIES OUTPUT_NAME_DEBUG sumo${BINARY_SUFFIX}D)
target_link_libraries(sumo microsim traciserver libsumostatic ${sumolibs})
if (JPS_FOUND)
    target_link_libraries(sumo jps)
endif()
add_dependencies(sumo generate-version-h)

if (FOX_FOUND)
    add_executable(sumo-gui guisim_main.cpp sumo-gui.rc)
    set_target_properties(sumo-gui PROPERTIES OUTPUT_NAME sumo-gui${BINARY_SUFFIX})
    set_target_properties(sumo-gui PROPERTIES OUTPUT_NAME_DEBUG sumo-gui${BINARY_SUFFIX}D)
    target_link_libraries(sumo-gui gui guinetload guisim gui gui_dialogs
        utils_gui_windows utils_gui_globjects utils_gui_div utils_gui_settings
        utils_gui_images utils_gui_cursors utils_gui_shortcuts utils_gui_tracker utils_foxtools utils_traction_wire ${osgviewlib} microsim traciserver libsumostatic ${sumolibs} mesogui
        ${GDAL_LIBRARY} ${FFMPEG_LIBRARIES} ${OPENSCENEGRAPH_LIBRARIES} ${GL2PS_LIBRARIES})
    if (MSVC)
        if (CONSOLE_RELEASE)
            set_target_properties(sumo-gui PROPERTIES LINK_FLAGS_RELEASE "/ENTRY:mainCRTStartup")
        else ()
            set_target_properties(sumo-gui PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
        endif()
        if (FFMPEG_FOUND)
            set_target_properties(sumo-gui PROPERTIES LINK_FLAGS "/SAFESEH:NO")
        endif ()
        target_link_libraries(sumo-gui opengl32 glu32)
    endif ()
    add_dependencies(sumo-gui generate-version-h)
    install(TARGETS sumo-gui RUNTIME DESTINATION bin)
endif ()

add_executable(netconvert netconvert_main.cpp)
set_target_properties(netconvert PROPERTIES OUTPUT_NAME netconvert${BINARY_SUFFIX})
set_target_properties(netconvert PROPERTIES OUTPUT_NAME_DEBUG netconvert${BINARY_SUFFIX}D)
target_link_libraries(netconvert ${netconvertlibs})
add_dependencies(netconvert generate-version-h)

add_executable(od2trips od2trips_main.cpp)
set_target_properties(od2trips PROPERTIES OUTPUT_NAME od2trips${BINARY_SUFFIX})
set_target_properties(od2trips PROPERTIES OUTPUT_NAME_DEBUG od2trips${BINARY_SUFFIX}D)
target_link_libraries(od2trips od router ${commonvehiclelibs})
add_dependencies(od2trips generate-version-h)

install(TARGETS sumo netconvert od2trips RUNTIME DESTINATION bin)
install(DIRECTORY libsumo DESTINATION include
        FILES_MATCHING PATTERN "*.h"
        PATTERN "*Helper.h" EXCLUDE
        PATTERN "Subscription.h" EXCLUDE)

configure_file(config.h.cmake config.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/config.h
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sumo
            COMPONENT Development)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../.git/index")
    set(VERSION_DEP "${CMAKE_CURRENT_SOURCE_DIR}/../.git/index")
endif()
add_custom_command(OUTPUT version.h
                   COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../tools/build/version.py ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS ${VERSION_DEP}
                  )
add_custom_target(generate-version-h DEPENDS version.h)
set_property(TARGET generate-version-h PROPERTY FOLDER "CMake")

# Create both a build and install CMake export package
set(export_config_name ${PROJECT_NAME})
set(${PROJECT_NAME}_INSTALL_CONFIG_DIR "lib/cmake/${PROJECT_NAME}")
file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${${PROJECT_NAME}_INSTALL_CONFIG_DIR})
#------------------------------------------------------------------------------
# Configure ${PROJECT_NAME}ConfigVersion.cmake common to build and install tree
include(CMakePackageConfigHelpers)
set(config_version_file ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake)
write_basic_package_version_file(
  ${config_version_file}
  VERSION "${PACKAGE_VERSION}"
  COMPATIBILITY ExactVersion
  )
#------------------------------------------------------------------------------
# Export '${PROJECT_NAME}Targets.cmake' for a build tree
export(
  EXPORT ${PROJECT_NAME}Targets
  NAMESPACE SUMO::
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
  )

# Configure '${PROJECT_NAME}Config.cmake' for a build tree
set(build_config ${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake)
configure_package_config_file(
  ${PROJECT_NAME}Config.cmake.in
  ${build_config}
  INSTALL_DESTINATION "${PROJECT_BINARY_DIR}"
  )

#------------------------------------------------------------------------------
# Export '${PROJECT_NAME}Targets.cmake' for an install tree
install(
  EXPORT ${PROJECT_NAME}Targets
  NAMESPACE SUMO::
  FILE ${PROJECT_NAME}Targets.cmake
  DESTINATION ${${PROJECT_NAME}_INSTALL_CONFIG_DIR}
  )

set(install_config ${PROJECT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake)
configure_package_config_file(
  ${PROJECT_NAME}Config.cmake.in
  ${install_config}
  INSTALL_DESTINATION ${${PROJECT_NAME}_INSTALL_CONFIG_DIR}
  )

# Install config files
install(
  FILES ${config_version_file} ${install_config}
  DESTINATION "${${PROJECT_NAME}_INSTALL_CONFIG_DIR}"
  )
