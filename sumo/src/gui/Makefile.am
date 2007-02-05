noinst_LIBRARIES = libgui.a

libgui_a_SOURCES = GUIAddWeightsStorage.h \
GUIApplicationWindow.cpp GUIApplicationWindow.h \
GUIColorer_LaneByPurpose.h GUIColorer_LaneByVehKnowledge.h GUIColorer_LaneNeighEdges.h \
GUIEvent_SimulationLoaded.h \
GUIGlobals.cpp GUIGlobals.h \
GUIManipulator.h GUIManipulator.cpp \
GUILoadThread.cpp GUILoadThread.h \
GUIRunThread.cpp GUIRunThread.h \
GUISUMOViewParent.cpp GUISUMOViewParent.h \
GUISupplementaryWeightsHandler.cpp GUISupplementaryWeightsHandler.h \
GUIThreadFactory.cpp GUIThreadFactory.h \
GUIViewTraffic.cpp GUIViewTraffic.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(FOX_INCLUDES) \
-I$(XERCES_INCLUDES)

SUBDIRS = dialogs drawerimpl






