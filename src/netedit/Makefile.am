EXTRA_DIST = netedit64.ico netedit.rc

XERCES_LIBS = -l$(LIB_XERCES)

if WITH_GUI
GUI_APPS = netedit
endif

bin_PROGRAMS = $(GUI_APPS)

if WITH_GUI
netedit_SOURCES = netedit_main.cpp \
GNEApplicationWindow.cpp GNEApplicationWindow.h \
GNELoadThread.cpp GNELoadThread.h \
GNENet.cpp GNENet.h \
GNEViewNet.cpp GNEViewNet.h \
GNEViewNetHelper.cpp GNEViewNetHelper.h \
GNEViewParent.cpp GNEViewParent.h \
GNEUndoList.cpp GNEUndoList.h \
GNEAttributeCarrier.cpp GNEAttributeCarrier.h \
GNEHierarchicalElementParents.cpp GNEHierarchicalElementParents.h \
GNEHierarchicalElementChildren.cpp GNEHierarchicalElementChildren.h \
GNEReferenceCounter.h \
GNEEvent_NetworkLoaded.h

SUBDIRS = additionals changes dialogs frames netelements demandelements

netedit_LDADD = ../netimport/libnetimport.a \
../netimport/vissim/libvissimimport.a \
../netimport/vissim/typeloader/libvissimtypeload.a \
../netimport/vissim/tempstructs/libvissimtmpstr.a \
../netwrite/libnetwrite.a \
../netbuild/libnetbuild.a \
../utils/distribution/libdistribution.a \
../foreign/eulerspiral/libeulerspiral.a \
../utils/distribution/libdistribution.a \
../foreign/eulerspiral/libeulerspiral.a \
../utils/geom/libgeom.a \
../utils/shapes/libshapes.a \
../utils/options/liboptions.a \
../utils/common/libcommon.a \
../utils/emissions/libemissions.a \
../utils/xml/libxml.a \
../utils/importio/libimportio.a \
../utils/iodevices/libiodevices.a \
../foreign/tcpip/libtcpip.a \
../netedit/additionals/libneteditadditionals.a \
../netedit/frames/libneteditframes.a \
../netedit/dialogs/libneteditdialogs.a \
../utils/gui/windows/libguiutilsWindows.a \
../utils/foxtools/libfoxtools.a \
../netedit/netelements/libneteditnetelements.a \
../netedit/demandelements/libneteditdemandelements.a \
../netedit/changes/libneteditchanges.a \
../utils/gui/globjects/libguiutilsglobjects.a \
../utils/gui/div/libguiutilsdiv.a \
../utils/gui/settings/libguiutilssettings.a \
../utils/gui/images/libguiutilsimages.a \
../utils/gui/cursors/libguiutilscursors.a \
../utils/gui/shortcuts/libguiutilsshortcuts.a \
../utils/gui/tracker/libguiutilstracker.a \
../utils/vehicle/libvehicle.a \
../utils/emissions/libemissions.a \
../foreign/PHEMlight/cpp/libphemlight.a \
$(XERCES_LIBS) $(FFMPEG_LIBS) \
$(FOX_LDFLAGS) $(XERCES_LDFLAGS) $(PROJ_LDFLAGS) $(GDAL_LDFLAGS) $(X_LDFLAGS)
endif
