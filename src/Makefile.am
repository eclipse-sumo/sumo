XERCES_LIBS = -l$(LIB_XERCES)

if WITH_GUI
GUI_APPS = sumo-gui
GUI_DIRS = gui guinetload guisim netedit mesogui osgview
endif


bin_PROGRAMS = od2trips sumo netconvert $(GUI_APPS)

COMMON_LIBS = ./utils/options/liboptions.a \
./utils/xml/libxml.a \
./utils/distribution/libdistribution.a \
./utils/geom/libgeom.a \
./utils/common/libcommon.a \
./utils/importio/libimportio.a \
./utils/iodevices/libiodevices.a \
./foreign/PHEMlight/cpp/libphemlight.a \
./foreign/tcpip/libtcpip.a \
$(XERCES_LIBS)


od2trips_SOURCES = od2trips_main.cpp

od2trips_LDADD   = ./od/libod.a \
./utils/options/liboptions.a \
./utils/vehicle/libvehicle.a \
$(COMMON_LIBS) $(XERCES_LDFLAGS)


sumo_SOURCES = sumo_main.cpp

sumo_LDADD   = ./netload/libnetload.a \
./microsim/libmicrosim.a \
./microsim/cfmodels/libmicrosimcfmodels.a \
./microsim/engine/libmicrosimengine.a \
./microsim/lcmodels/libmicrosimlcmodels.a \
./microsim/devices/libmicrosimdevs.a \
./microsim/output/libmicrosimoutput.a \
./microsim/MSMoveReminder.o \
./microsim/trigger/libmicrosimtrigger.a \
./microsim/actions/libmsactions.a \
./microsim/traffic_lights/libmicrosimtls.a \
./microsim/pedestrians/libmicrosimpeds.a \
./mesosim/libmesosim.a \
./utils/vehicle/libvehicle.a \
./utils/shapes/libshapes.a \
./utils/emissions/libemissions.a \
./traci-server/libtraciserver.a \
./libsumo/libsumostatic.a \
./utils/traci/libtraciclient.a \
$(COMMON_LIBS) \
$(PYTHON_LIBS) \
$(FOX_LDFLAGS) $(XERCES_LDFLAGS) $(PROJ_LDFLAGS)


netconvert_SOURCES = netconvert_main.cpp

netconvert_LDADD   = ./netimport/libnetimport.a \
./netimport/vissim/libvissimimport.a \
./netimport/vissim/typeloader/libvissimtypeload.a \
./netimport/vissim/tempstructs/libvissimtmpstr.a \
./netwrite/libnetwrite.a \
./netbuild/libnetbuild.a \
./utils/shapes/libshapes.a \
./foreign/eulerspiral/libeulerspiral.a \
$(COMMON_LIBS) \
$(XERCES_LDFLAGS) $(GDAL_LDFLAGS) $(PROJ_LDFLAGS)


if WITH_GUI
sumo_gui_SOURCES = guisim_main.cpp

sumo_gui_LDADD =  ./gui/libgui.a \
./guinetload/libguinetload.a \
./guisim/libguisim.a \
./gui/dialogs/libguidialogs.a \
./utils/gui/windows/libguiutilsWindows.a \
./utils/gui/globjects/libguiutilsglobjects.a \
./utils/gui/div/libguiutilsdiv.a \
./utils/gui/settings/libguiutilssettings.a \
./utils/gui/images/libguiutilsimages.a \
./utils/gui/cursors/libguiutilscursors.a \
./utils/gui/shortcuts/libguiutilsshortcuts.a \
./utils/gui/tracker/libguiutilstracker.a \
./utils/foxtools/libfoxtools.a \
./mesogui/libmesogui.a \
./osgview/libosgview.a \
$(sumo_LDADD) \
./gui/GUIManipulator.o \
./gui/GUITLLogicPhasesTrackerWindow.o \
$(OSG_LIBS) $(FFMPEG_LIBS) \
$(sumo_LDFLAGS) $(GDAL_LDFLAGS) $(X_LDFLAGS)
endif


SUBDIRS = foreign utils microsim router od marouter mesosim \
netbuild netwrite netgen netimport netload \
polyconvert dfrouter duarouter jtrrouter activitygen tools \
libsumo traci-server traci_testclient $(GUI_DIRS)

if WITH_VERSION_H
VERSION_H = version.h
all: $(VERSION_H)
$(VERSION_H): ../.git
	../tools/build/version.py $(CURDIR)
endif

EXTRA_DIST = config.h.in $(VERSION_H) sumo-gui.rc
