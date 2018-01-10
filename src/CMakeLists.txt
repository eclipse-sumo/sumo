set(netconvertlibs
        netwrite netimport netbuild foreign_eulerspiral ${GDAL_LIBRARY} netimport_vissim netimport_vissim_typeloader netimport_vissim_tempstructs ${commonlibs})

set(sumolibs
        netload microsim microsim_cfmodels microsim_lcmodels microsim_devices microsim_output microsim_pedestrians microsim_trigger microsim_actions
        microsim_traffic_lights mesosim traciserver libsumostatic ${commonvehiclelibs} ${GRPC_LIBS}
        ${PYTHON_LIBRARIES})

add_subdirectory(activitygen)
add_subdirectory(dfrouter)
add_subdirectory(duarouter)
add_subdirectory(foreign)
add_subdirectory(gui)
add_subdirectory(guinetload)
add_subdirectory(guisim)
add_subdirectory(jtrrouter)
add_subdirectory(libsumo)
add_subdirectory(marouter)
add_subdirectory(mesogui)
add_subdirectory(mesosim)
add_subdirectory(microsim)
add_subdirectory(netbuild)
add_subdirectory(netedit)
add_subdirectory(netgen)
add_subdirectory(netimport)
add_subdirectory(netload)
add_subdirectory(netwrite)
add_subdirectory(od)
add_subdirectory(osgview)
add_subdirectory(polyconvert)
add_subdirectory(router)
add_subdirectory(tools)
add_subdirectory(traci-server)
add_subdirectory(traci_testclient)
add_subdirectory(utils)


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(sumoexec
            sumo_main.cpp
            microsim/MSMoveReminder.cpp microsim/MSMoveReminder.h)
    set(sumoguiexec
            guisim_main.cpp
            microsim/MSMoveReminder.cpp microsim/MSMoveReminder.h
            gui/GUIManipulator.h gui/GUIManipulator.cpp
            gui/GUITLLogicPhasesTrackerWindow.h gui/GUITLLogicPhasesTrackerWindow.cpp
            mesosim/MEVehicleControl.h mesosim/MEVehicleControl.cpp)
else ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(sumoexec
            sumo_main.cpp)
    set(sumoguiexec
            guisim_main.cpp)
endif ()

add_executable(sumo version.h ${sumoexec})
set_target_properties(sumo PROPERTIES OUTPUT_NAME_DEBUG sumoD)
target_link_libraries(sumo ${sumolibs})

add_executable(sumo-gui version.h ${sumoguiexec} guisim.rc)
set_target_properties(sumo-gui PROPERTIES OUTPUT_NAME_DEBUG sumo-guiD)
target_link_libraries(sumo-gui gui guinetload guisim gui_dialogs
        utils_gui_windows utils_gui_globjects utils_gui_div utils_gui_settings
        utils_gui_images utils_gui_tracker utils_foxtools osgview ${sumolibs} mesogui
        ${GDAL_LIBRARY} ${FFMPEG_LIBRARIES} ${OPENSCENEGRAPH_LIBRARIES} ${GL2PS_LIBRARIES})
if (MSVC)
    set_target_properties(sumo-gui PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    target_link_libraries(sumo-gui opengl32 glu32)
endif ()

add_executable(netconvert version.h netconvert_main.cpp)
set_target_properties(netconvert PROPERTIES OUTPUT_NAME_DEBUG netconvertD)
target_link_libraries(netconvert ${netconvertlibs})

add_executable(od2trips version.h od2trips_main.cpp)
set_target_properties(od2trips PROPERTIES OUTPUT_NAME_DEBUG od2tripsD)
target_link_libraries(od2trips od utils_vehicle ${commonlibs})

configure_file(config.h.cmake config.h)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/src/version.h
                   COMMAND ${CMAKE_SOURCE_DIR}/tools/build/version.py ${CMAKE_BINARY_DIR}/src
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../.svn/wc.db
                  )
add_custom_target(generate-version-h
                  DEPENDS ${CMAKE_BINARY_DIR}/src/version.h
                 )
set_source_files_properties(${CMAKE_BINARY_DIR}/src/version.h
    PROPERTIES GENERATED TRUE)
