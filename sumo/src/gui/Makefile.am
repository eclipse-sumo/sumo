noinst_LIBRARIES = libgui.a


EXTRA_DIST =

libgui_a_SOURCES = GUIApplicationWindow.cpp GUIApplicationWindow.h \
GUIChooser.cpp GUIChooser.h \
GUIDanielPerspectiveChanger.cpp GUIDanielPerspectiveChanger.h \
GUIEvents.h GUIGlObject.cpp GUIGlObject.h \
GUIGlObjectStorage.cpp GUIGlObjectStorage.h \
GUIGlObjectTypes.h GUILoadThread.cpp GUILoadThread.h \
GUIPerspectiveChanger.cpp GUIPerspectiveChanger.h \
GUIRunThread.cpp GUIRunThread.h \
GUISUMOAbstractView.cpp GUISUMOAbstractView.h \
GUISUMOViewParent.cpp GUISUMOViewParent.h \
GUISimpleLaneDrawer.cpp GUISimpleLaneDrawer.h \
GUITriangleVehicleDrawer.cpp GUITriangleVehicleDrawer.h \
GUIViewTraffic.cpp GUIViewTraffic.h \
QAboutSUMO.cpp QAboutSUMO.h \
QGLObjectPopupMenu.cpp QGLObjectPopupMenu.h \
QGLObjectPopupMenuItem.cpp QGLObjectPopupMenuItem.h \
QGLObjectToolTip.cpp QGLObjectToolTip.h \
QGUIImageField.cpp QGUIImageField.h QGUIToggleButton.cpp QGUIToggleButton.h \
QSUMOEvent.h QSimulationLoadedEvent.h QSimulationStepEvent.h \
TableTypes.h \
GUIApplicationWindow.moc GUIChooser.moc GUISUMOAbstractView.moc \
GUISUMOViewParent.moc GUIViewTraffic.moc QAboutSUMO.moc \
QGLObjectPopupMenu.moc QGUIImageField.moc QGUIToggleButton.moc

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src $(all_includes) \
-I$(XERCESINCLUDE) -I$(XERCESINCLUDE_PRIOR_1_7)

SUBDIRS = icons partable vartracker

libgui_a_METASOURCES = GUIApplicationWindow.moc GUIChooser.moc \
GUISUMOAbstractView.moc GUISUMOViewParent.moc \
GUIViewTraffic.moc QAboutSUMO.moc \
QGLObjectPopupMenu.moc QGUIImageField.moc QGUIToggleButton.moc

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(libgui_a_METASOURCES)




