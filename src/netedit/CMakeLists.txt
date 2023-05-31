# First link all subdirectories
add_subdirectory(changes)
add_subdirectory(dialogs)
add_subdirectory(frames)
add_subdirectory(elements)
add_subdirectory(tools)

# create set with all main netedit source code files
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
target_link_libraries(netedit gui_dialogs utils_gui_cursors utils_gui_shortcuts utils_foxtools foreign_eulerspiral
        netedit_frames_common netedit_frames_network netedit_frames_demand netedit_frames_data netedit_frames 
        netedit_elements netedit_elements_network netedit_elements_additional netedit_elements_demand netedit_elements_data 
        netedit_changes utils_gui_div utils_gui_settings utils_gui_images netedit_dialogs_tools netedit_tools netedit_dialogs 
        utils_gui_windows utils_gui_globjects utils_gui_tracker utils_foxtools utils_vehicle utils_emissions
        foreign_phemlight foreign_phemlight_V5 ${netconvertlibs} ${FOX_LIBRARY} ${GL2PS_LIBRARIES})

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
# the python script <SUMO>/tools/build/templates.py (<SUMO/src/netedit>/../../tools/build/templates.py) and the sumo binary
# the LSAN_OPTIONS are only relevant for the clang build but should not hurt others
add_custom_command(OUTPUT templates.h
                   COMMAND ${CMAKE_COMMAND} -E env LSAN_OPTIONS=suppressions=${CMAKE_CURRENT_SOURCE_DIR}/../../build/clang_memleak_suppressions.txt ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/build/templates.py $<TARGET_FILE:sumo> $<TARGET_FILE:netgenerate>
                   DEPENDS sumo netgenerate
                   )
				   
# Finally mark the generated templates.h as a generated file
set_source_files_properties(${CMAKE_BINARY_DIR}/src/netedit/templates.h PROPERTIES GENERATED TRUE)
