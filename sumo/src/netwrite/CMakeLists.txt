message("entering netwrite")


set(netwrite_files
        NWFrame.cpp NWFrame.h
        NWWriter_Amitran.cpp NWWriter_Amitran.h
        NWWriter_DlrNavteq.cpp NWWriter_DlrNavteq.h
        NWWriter_MATSim.cpp NWWriter_MATSim.h
        NWWriter_OpenDrive.cpp NWWriter_OpenDrive.h
        NWWriter_SUMO.cpp NWWriter_SUMO.h
        NWWriter_XML.cpp NWWriter_XML.h
        )

add_library(nw STATIC ${netwrite_files})