add_subdirectory(additionals)
add_subdirectory(changes)
add_subdirectory(dialogs)
add_subdirectory(frames)
add_subdirectory(netelements)

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
   GNEViewParent.h
   GNEViewParent.cpp
   GNEUndoList.h
   GNEUndoList.cpp
   GNEAttributeCarrier.h
   GNEAttributeCarrier.cpp
   GNEReferenceCounter.h
   GNEEvent_NetworkLoaded.h
)

add_executable(netedit ${netedit_SRCS} netedit.rc)
set_target_properties(netedit PROPERTIES OUTPUT_NAME_DEBUG neteditD)
target_link_libraries(netedit utils_gui_windows utils_gui_globjects utils_gui_div utils_gui_settings utils_gui_images utils_gui_tracker
        utils_foxtools foreign_eulerspiral netedit_frames netedit_changes netedit_netelements netedit_additionals netedit_dialogs utils_gui_windows utils_foxtools ${netconvertlibs} ${FOX_LIBRARY} ${GL2PS_LIBRARIES})
if (MSVC)
    set_target_properties(netedit PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    target_link_libraries(netedit opengl32 glu32)
endif ()

install(TARGETS netedit RUNTIME DESTINATION bin)
