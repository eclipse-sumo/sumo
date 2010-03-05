XERCES_LIBS = -l$(LIB_XERCES)

if WITH_PROJ
PROJ_LIBS = -l$(LIB_PROJ)
endif

if WITH_GDAL
GDAL_LIBS = -l$(LIB_GDAL)
endif

if MESOSIM
MESO_LIBS = ./mesosim/libmesosim.a
MESOGUI_LIBS = ./mesogui/libmesogui.a
endif

if WITH_GUI
GUI_APPS = sumo-gui
GUI_DIRS = $(MESOGUI_DIRS) gui guinetload guisim icons
endif

if CHECK_MEMORY_LEAKS
MEM_LIBS  = ./foreign/nvwa/libnvwa.a
endif

if TRACI
TRACI_LIBS = ./traci-server/libtraciserver.a
TRACI_DIRS = traci-server traci_testclient

TRACI_TESTCLIENT_APPS = traci-testclient
traci_testclient_SOURCES = ./traci_testclient/tracitestclient_main.cpp
traci_testclient_LDADD = ./traci_testclient/libtracitestclient.a\
./foreign/tcpip/libtcpip.a
endif

bin_PROGRAMS = sumo netconvert \
duarouter dfrouter jtrrouter \
$(GUI_APPS) $(TRACI_TESTCLIENT_APPS)

COMMON_LIBS = ./utils/options/liboptions.a \
./utils/common/libcommon.a \
./utils/xml/libxml.a \
./utils/importio/libimportio.a \
./utils/iodevices/libiodevices.a \
./foreign/tcpip/libtcpip.a \
$(MEM_LIBS) \
$(XERCES_LIBS)


sumo_SOURCES = sumo_main.cpp

sumo_LDFLAGS = $(XERCES_LDFLAGS) $(GDAL_LDFLAGS) $(PROJ_LDFLAGS)

sumo_LDADD   = ./netload/libnetload.a \
./microsim/libmicrosim.a \
./microsim/devices/libmicrosimdevs.a \
./microsim/output/libmicrosimoutput.a \
./microsim/MSMoveReminder.o \
./microsim/trigger/libmicrosimtrigger.a \
./microsim/actions/libmsactions.a \
./microsim/traffic_lights/libmicrosimtls.a \
$(MESO_LIBS) \
./utils/geom/libgeom.a \
./utils/shapes/libshapes.a \
$(COMMON_LIBS) \
$(TRACI_LIBS) \
$(PROJ_LIBS)



netconvert_SOURCES = netconvert_main.cpp

netconvert_LDFLAGS = $(XERCES_LDFLAGS) $(GDAL_LDFLAGS) $(PROJ_LDFLAGS)

netconvert_LDADD   = ./netimport/libnetimport.a \
./netimport/vissim/libvissimimport.a \
./netimport/vissim/typeloader/libvissimtypeload.a \
./netimport/vissim/tempstructs/libvissimtmpstr.a \
./netbuild/libnetbuild.a \
./utils/distribution/libdistribution.a \
./utils/geom/libgeom.a \
./foreign/eulerspiral/libeulerspiral.a \
$(COMMON_LIBS) \
$(GDAL_LIBS) \
$(PROJ_LIBS)



duarouter_SOURCES = duarouter_main.cpp

duarouter_LDFLAGS = $(XERCES_LDFLAGS)

duarouter_LDADD   = ./routing_dua/libduarouting.a \
./router/librouter.a \
$(COMMON_LIBS)


dfrouter_SOURCES = dfrouter_main.cpp

dfrouter_LDFLAGS = $(XERCES_LDFLAGS)

dfrouter_LDADD   = ./routing_df/libdfrouting.a \
./router/librouter.a \
./utils/geom/libgeom.a \
$(COMMON_LIBS)


jtrrouter_SOURCES = jtrrouter_main.cpp

jtrrouter_LDFLAGS = $(XERCES_LDFLAGS)

jtrrouter_LDADD   = ./routing_jtr/libjtrrouting.a \
./router/librouter.a \
$(COMMON_LIBS)


if WITH_GUI
sumo_gui_SOURCES = guisim_main.cpp

sumo_gui_LDFLAGS = $(FOX_LDFLAGS) $(sumo_LDFLAGS) $(X_LDFLAGS)

sumo_gui_LDADD =  ./gui/libgui.a \
./guinetload/libguinetload.a \
./guisim/libguisim.a \
./gui/dialogs/libguidialogs.a \
./utils/gui/windows/libguiutilsWindows.a \
./utils/gui/globjects/libguiutilsglobjects.a \
./utils/gui/div/libguiutilsdiv.a \
./utils/gui/settings/libguiutilssettings.a \
./utils/gui/events/libguiutilsevents.a \
./utils/gui/images/libguiutilsimages.a \
./utils/gui/tracker/libguiutilstracker.a \
./utils/foxtools/libfoxtools.a \
$(MESOGUI_LIBS) \
$(sumo_LDADD) \
./foreign/polyfonts/libpolyfonts.a \
./gui/GUIManipulator.o \
./gui/GUITLLogicPhasesTrackerWindow.o \
-l$(LIB_FOX)
endif


SUBDIRS = foreign microsim netbuild netgen netimport netload \
od2trips polyconvert router routing_df routing_dua routing_jtr \
utils $(GUI_DIRS) $(MESO_DIRS) $(TRACI_DIRS)

EXTRA_DIST = config.h config.h.in \
guisim.rc \
windows_config.h

if WITH_VERSION_H
all: version.h
version.h: .svn/entries
	../tools/build/version.py
endif
