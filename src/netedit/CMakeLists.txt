add_subdirectory(additionals)
add_subdirectory(changes)
add_subdirectory(dialogs)
add_subdirectory(frames)
add_subdirectory(netelements)
add_subdirectory(demandelements)

set(netedit_SRCS
   netedit_main.cpp
   GNEApplicationWindow.h
   GNEApplicationWindow.cpp
   GNELoadThread.h
   GNELoadThread.cpp
   GNENet.h
   GNENet.cpp
   GNEViewNet.h
   GNEViewNet.cpp
   GNEViewNetHelper.h
   GNEViewNetHelper.cpp
   GNEViewParent.h
   GNEViewParent.cpp
   GNEUndoList.h
   GNEUndoList.cpp
   GNEAttributeCarrier.h
   GNEAttributeCarrier.cpp
   GNEHierarchicalElementParents.h
   GNEHierarchicalElementParents.cpp
   GNEHierarchicalElementChildren.h
   GNEHierarchicalElementChildren.cpp
   GNEReferenceCounter.h
   GNEEvent_NetworkLoaded.h
)

add_executable(netedit ${netedit_SRCS} netedit.rc)
set_target_properties(netedit PROPERTIES OUTPUT_NAME netedit${BINARY_SUFFIX})
set_target_properties(netedit PROPERTIES OUTPUT_NAME_DEBUG netedit${BINARY_SUFFIX}D)
target_link_libraries(netedit utils_gui_cursors utils_gui_shortcuts 
        utils_foxtools foreign_eulerspiral netedit_frames
        netedit_netelements netedit_demandelements netedit_changes utils_gui_div
        utils_gui_settings utils_gui_images netedit_additionals netedit_dialogs
        utils_gui_windows utils_gui_globjects utils_gui_tracker utils_foxtools utils_vehicle utils_emissions foreign_phemlight ${netconvertlibs} ${FOX_LIBRARY} ${GL2PS_LIBRARIES})
if (MSVC)
    set_target_properties(netedit PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    target_link_libraries(netedit opengl32 glu32)
endif ()
add_dependencies(netedit generate-version-h)

install(TARGETS netedit RUNTIME DESTINATION bin)
