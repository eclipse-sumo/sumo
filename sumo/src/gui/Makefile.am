noinst_LIBRARIES = libgui.a


EXTRA_DIST =

libgui_a_SOURCES = GUIApplicationWindow.cpp GUIApplicationWindow.h \
GUIChooser.cpp GUIChooser.h GUILoadThread.cpp GUILoadThread.h \
GUIRunThread.cpp GUIRunThread.h GUISUMOView.cpp GUISUMOView.h \
GUIViewTraffic.cpp GUIViewTraffic.h QGUIToggleButton.h

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src $(all_includes)

SUBDIRS = icons

libgui_a_METASOURCES = AUTO

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(libgui_a_METASOURCES)




