# First link all subdirectories
add_subdirectory(changes)
add_subdirectory(dialogs)
add_subdirectory(frames)
add_subdirectory(elements)
add_subdirectory(tools)

# create set with all main netedit source code files
set(netedit_SRCS
   GNEApplicationWindow.cpp
   GNEApplicationWindow.h
   GNEApplicationWindowHelper.cpp
   GNEApplicationWindowHelper.h
   GNEAttributeProperties.cpp
   GNEAttributeProperties.h
   GNEEvent_NetworkLoaded.h
   GNEExternalRunner.cpp
   GNEExternalRunner.h
   GNEInternalTest.cpp
   GNEInternalTest.h
   GNELane2laneConnection.cpp
   GNELane2laneConnection.h
   GNELoadThread.cpp
   GNELoadThread.h
   GNENet.cpp
   GNENet.h
   GNENetHelper.cpp
   GNENetHelper.h
   GNEPathManager.cpp
   GNEPathManager.h
   GNEReferenceCounter.h
   GNESegment.cpp
   GNESegment.h
   GNETagProperties.cpp
   GNETagProperties.h
   GNETagPropertiesDatabase.cpp
   GNETagPropertiesDatabase.h
   GNEUndoList.cpp
   GNEUndoList.h
   GNEViewNet.cpp
   GNEViewNet.h
   GNEViewNetHelper.cpp
   GNEViewNetHelper.h
   GNEViewParent.cpp
   GNEViewParent.h
   netedit_main.cpp
   templates.h
)

# create netedit executable using the main netedit source code files defined previously in netedit_SRCS
# and icon defined in file netedit.rc
add_executable(netedit ${netedit_SRCS} netedit.rc)

# Add suffix to netedit executable in RELEASE mode if was defined in variable BINARY_SUFFIX
set_target_properties(netedit PROPERTIES OUTPUT_NAME netedit${BINARY_SUFFIX})

# Add suffix + D to netedit executable in DEBUG mode if was defined in variable BINARY_SUFFIX
set_target_properties(netedit PROPERTIES OUTPUT_NAME_DEBUG netedit${BINARY_SUFFIX}D)

# link all previously compiled libraries to netedit. Every library was compiled in their own folder using own CMakeList.txt
target_link_libraries(netedit gui_dialogs utils_gui_cursors utils_gui_shortcuts utils_tests utils_foxtools foreign_eulerspiral
        netedit_frames_common netedit_frames_network netedit_frames_demand netedit_frames_data netedit_frames
        netedit_elements netedit_elements_network netedit_elements_additional netedit_elements_demand netedit_elements_moving
        netedit_elements_data netedit_changes utils_gui_div utils_gui_settings utils_gui_images netedit_dialogs_run
        netedit_dialogs_tools netedit_dialogs_basic netedit_dialogs_file netedit_dialogs_fix netedit_dialogs_elements
        netedit_dialogs_elements_lists netedit_dialogs_options netedit_tools netedit_dialogs utils_gui_windows utils_gui_globjects
        utils_gui_tracker utils_foxtools utils_vehicle utils_emissions foreign_phemlight foreign_phemlight_V5
        ${netconvertlibs} ${FOX_LIBRARY} ${GL2PS_LIBRARIES} ${BOOST_LIBRARIES})

# if we're compiling using Visual Studio, add extra options
if (MSVC)
    # Enable or disable console in release mode depending of variable CONSOLE_RELEASE
    # and set VC variable ENTRY, see https://learn.microsoft.com/en-us/cpp/build/reference/entry-entry-point-symbol
    if (CONSOLE_RELEASE)
        set_target_properties(netedit PROPERTIES LINK_FLAGS_RELEASE "/ENTRY:mainCRTStartup")
    else ()
        set_target_properties(netedit PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    endif()
    # Disable DPI Aware if we're using a old CMAKE Version
    if (CMAKE_VERSION VERSION_LESS "3.16.0")
        message(STATUS "DPI awareness for netedit will be disabled. Please consider using CMake version >= 3.16 to enable it")
    else ()
        set_target_properties(netedit PROPERTIES VS_DPI_AWARE "ON")
    endif ()
    # Target system libraries opengl32 and glu32 (common for all operating systems)
    target_link_libraries(netedit opengl32 glu32)
endif ()

# define dependencies of NETEDIT (needs templates.h, version.h and DLLs installed in bin folder)
add_dependencies(netedit generate-templates-h generate-version-h install_dll)

# finally, define output folder (in our case, SUMO/bin)
install(TARGETS netedit RUNTIME DESTINATION bin)

# now we're going to define the templates.h dependency.
# First we define a custom target for templates.h
add_custom_target(generate-templates-h DEPENDS templates.h)

# place custom target in tree folder CMake (in Visual studio)
set_property(TARGET generate-templates-h PROPERTY FOLDER "CMake")

# for generating dependences we need a SUMO binary compiled, then add sumo as dependency of generate-templates-h.
# this will compile SUMO BEFORE generate templates.h
add_dependencies(generate-templates-h sumo netgenerate)

# create CMake custom command that generates templates.h file (OUTPUT) giving as argument python executable (PYTHON_EXECUTABLE),
# the python script <SUMO>/tools/build_config/templates.py (<SUMO/src/netedit>/../../tools/build_config/templates.py) and the sumo binary
add_custom_command(OUTPUT templates.h
                   COMMAND ${PYTHON_EXECUTABLE} ${SUMO_TOOLS_DIR}/build_config/templates.py $<TARGET_FILE:sumo> $<TARGET_FILE:netgenerate>
                   DEPENDS sumo netgenerate
                   )

# Finally mark the generated templates.h as a generated file
set_source_files_properties(${CMAKE_BINARY_DIR}/src/netedit/templates.h PROPERTIES GENERATED TRUE)
