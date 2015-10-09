noinst_LIBRARIES = libnetimport.a

libnetimport_a_SOURCES = NIFrame.cpp NIFrame.h \
NILoader.cpp NILoader.h \
NIImporter_ArcView.cpp NIImporter_ArcView.h \
NIImporter_DlrNavteq.cpp NIImporter_DlrNavteq.h \
NIImporter_ITSUMO.cpp NIImporter_ITSUMO.h \
NIImporter_MATSim.cpp NIImporter_MATSim.h \
NIImporter_OpenDrive.cpp NIImporter_OpenDrive.h \
NIImporter_OpenStreetMap.cpp NIImporter_OpenStreetMap.h \
NIImporter_RobocupRescue.cpp NIImporter_RobocupRescue.h \
NIImporter_SUMO.cpp NIImporter_SUMO.h \
NIImporter_VISUM.cpp NIImporter_VISUM.h \
NIVisumTL.cpp NIVisumTL.h \
NIXMLConnectionsHandler.cpp NIXMLConnectionsHandler.h \
NIXMLEdgesHandler.cpp NIXMLEdgesHandler.h \
NIXMLNodesHandler.cpp NIXMLNodesHandler.h \
NIXMLTypesHandler.cpp NIXMLTypesHandler.h \
NIXMLTrafficLightsHandler.cpp NIXMLTrafficLightsHandler.h \
NINavTeqHelper.cpp NINavTeqHelper.h

SUBDIRS = vissim

BUILT_SOURCES = typemap.h
EXTRA_DIST = typemap.h
typemap.h:
	../../tools/build/typemap.py
