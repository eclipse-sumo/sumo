noinst_LIBRARIES = libguipartable.a


EXTRA_DIST =

libguipartable_a_SOURCES = GUIParameterTable.cpp GUIParameterTable.h \
GUIParameterTableWindow.cpp GUIParameterTableWindow.h \
QParamPopupMenu.cpp QParamPopupMenu.h \
GUIParameterTable.moc GUIParameterTableWindow.moc QParamPopupMenu.moc

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src $(all_includes) \
-I$(XERCESINCLUDE) -I$(XERCESINCLUDE_PRIOR_1_7)

SUBDIRS =

libguipartable_a_METASOURCES = GUIParameterTable.moc \
GUIParameterTableWindow.moc QParamPopupMenu.moc

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(libguipartable_a_METASOURCES)




