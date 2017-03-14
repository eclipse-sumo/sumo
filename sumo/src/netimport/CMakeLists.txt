message("entering netimport")

#add_subdirectory(vissim)

set(netimport_files
     NIFrame.cpp NIFrame.h
        NIImporter_ArcView.cpp NIImporter_ArcView.h
        NIImporter_DlrNavteq.cpp NIImporter_DlrNavteq.h
        NIImporter_ITSUMO.cpp NIImporter_ITSUMO.h
        NIImporter_MATSim.cpp NIImporter_MATSim.h
        NIImporter_OpenDrive.cpp NIImporter_OpenDrive.h
        NIImporter_OpenStreetMap.cpp NIImporter_OpenStreetMap.h
        NIImporter_RobocupRescue.cpp NIImporter_RobocupRescue.h
        NIImporter_SUMO.cpp NIImporter_SUMO.h
        NIImporter_VISUM.cpp NIImporter_VISUM.h
        NILoader.cpp NILoader.h
        NINavTeqHelper.cpp NINavTeqHelper.h
        NIVisumTL.cpp NIVisumTL.h
        NIXMLConnectionsHandler.cpp NIXMLConnectionsHandler.h
        NIXMLEdgesHandler.cpp NIXMLEdgesHandler.h
        NIXMLNodesHandler.cpp NIXMLNodesHandler.h
        NIXMLTrafficLightsHandler.cpp NIXMLTrafficLightsHandler.h
        NIXMLTypesHandler.cpp NIXMLTypesHandler.h
        typemap.h
        )

add_library(ni STATIC ${netimport_files})