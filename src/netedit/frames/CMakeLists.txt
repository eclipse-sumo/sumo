add_subdirectory(commonframes)
add_subdirectory(netframes)
add_subdirectory(demandframes)

set(netedit_frames_SRCS
   GNEFrame.h
   GNEFrame.cpp
   GNEFrameModuls.h
   GNEFrameModuls.cpp
   GNEFrameAttributesModuls.h
   GNEFrameAttributesModuls.cpp
   )

add_library(netedit_frames STATIC ${netedit_frames_SRCS})
set_property(TARGET netedit_frames PROPERTY PROJECT_LABEL "z_netedit_frames")