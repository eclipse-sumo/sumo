noinst_LIBRARIES = libgui.a


EXTRA_DIST =

libgui_a_SOURCES = GUIApplicationWindow.h \
GUIChooser.h \
GUIDanielPerspectiveChanger.h \
GUIEvents.h \
GUIGlObject.h \
GUIGlObjectStorage.h \
GUILoadThread.h \
GUIPerspectiveChanger.h \
GUIRunThread.h \
GUISimpleLaneDrawer.h \
GUISUMOView.h \
GUITriangleVehicleDrawer.h \
GUIViewTraffic.h \
QAboutSUMO.h \
QGLObjectToolTip.h \
QGUIImageField.h \
QGUIToggleButton.h \
QSimulationLoadedEvent.h \
QSimulationStepEvent.h \
QSUMOEvent.h \
GUIApplicationWindow.cpp \
GUIChooser.cpp \
GUIDanielPerspectiveChanger.cpp \
GUIGlObject.cpp \
GUIGlObjectStorage.cpp \
GUILoadThread.cpp \
GUIPerspectiveChanger.cpp \
GUIRunThread.cpp \
GUISimpleLaneDrawer.cpp \
GUISUMOView.cpp \
GUITriangleVehicleDrawer.cpp \
GUIViewTraffic.cpp \
QAboutSUMO.cpp \
QGLObjectToolTip.cpp

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src $(all_includes) \
-I$(XERCESINCLUDE) -I$(XERCESINCLUDE_PRIOR_1_7)

SUBDIRS = icons

libgui_a_METASOURCES = AUTO

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(libgui_a_METASOURCES)




