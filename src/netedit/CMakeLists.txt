add_subdirectory(changes)
add_subdirectory(dialogs)
add_subdirectory(frames)
add_subdirectory(elements)

set(netedit_SRCS
   netedit_main.cpp
   GNEApplicationWindowHelper.h
   GNEApplicationWindowHelper.cpp
   GNEApplicationWindow.h
   GNEApplicationWindow.cpp
   GNELoadThread.h
   GNELoadThread.cpp
   GNENet.h
   GNENet.cpp
   GNENetHelper.h
   GNENetHelper.cpp
   GNEViewNet.h
   GNEViewNet.cpp
   GNEViewNetHelper.h
   GNEViewNetHelper.cpp
   GNEViewParent.h
   GNEViewParent.cpp
   GNEUndoList.h
   GNEUndoList.cpp
   GNEMoveElement.h
   GNEMoveElement.cpp
   GNEPathManager.h
   GNEPathManager.cpp
   GNEReferenceCounter.h
   GNEEvent_NetworkLoaded.h
   GNELane2laneConnection.h
   GNELane2laneConnection.cpp
)

add_executable(netedit ${netedit_SRCS} netedit.rc)
set_target_properties(netedit PROPERTIES OUTPUT_NAME netedit${BINARY_SUFFIX})
set_target_properties(netedit PROPERTIES OUTPUT_NAME_DEBUG netedit${BINARY_SUFFIX}D)
target_link_libraries(netedit utils_gui_cursors utils_gui_shortcuts utils_foxtools foreign_eulerspiral
        netedit_frames_common netedit_frames_network netedit_frames_demand netedit_frames_data netedit_frames 
        netedit_elements_network netedit_elements_additional netedit_elements_demand netedit_elements_data 
        netedit_elements netedit_changes utils_gui_div utils_gui_settings utils_gui_images netedit_dialogs 
        netedit_dialogs_tools utils_gui_windows utils_gui_globjects utils_gui_tracker utils_foxtools 
        utils_vehicle utils_emissions foreign_phemlight foreign_phemlight_V5 ${netconvertlibs} ${FOX_LIBRARY} ${GL2PS_LIBRARIES})
if (MSVC)
    if (CONSOLE_RELEASE)
        set_target_properties(netedit PROPERTIES LINK_FLAGS_RELEASE "/ENTRY:mainCRTStartup")
    else ()
        set_target_properties(netedit PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    endif()
    if (CMAKE_VERSION VERSION_LESS "3.16.0")
        message(STATUS "DPI awareness for netedit will be disabled. Please consider using CMake version >= 3.16 to enable it")
    else ()
        set_target_properties(netedit PROPERTIES VS_DPI_AWARE "ON")
    endif ()
    target_link_libraries(netedit opengl32 glu32)
endif ()
add_dependencies(netedit generate-version-h install_dll)

install(TARGETS netedit RUNTIME DESTINATION bin)
