XERCES_LIBS = -l$(LIB_XERCES)

if WITH_PROJ
PROJ_LIBS = -l$(LIB_PROJ)
endif

if WITH_GDAL
GDAL_LIBS = -l$(LIB_GDAL)
endif

if WITH_GUI
GUI_APPS = sumo-guisim
GUI_DIRS = gui guinetload guisim icons
endif

if CHECK_MEMORY_LEAKS
MEM_LIBS  = ./foreign/nvwa/libnvwa.a
endif

if ITM
TCPIP_LIBS = ./foreign/tcpip/libtcpip.a
ITM_LIBS = ./itm-remoteserver/libitmremoteserver.a
ITM_DIRS = itm-remoteserver
endif

bin_PROGRAMS = sumo sumo-netconvert sumo-netgen \
sumo-duarouter sumo-dfrouter sumo-jtrrouter \
sumo-od2trips sumo-polyconvert $(GUI_APPS)

COMMON_LIBS = ./utils/options/liboptions.a \
./utils/common/libcommon.a \
./utils/xml/libxml.a \
./utils/gfx/libgfx.a \
./utils/importio/libimportio.a \
./utils/iodevices/libiodevices.a

sumo_SOURCES = sumo_main.cpp

sumo_LDFLAGS = $(XERCES_LDFLAGS) $(GDAL_LDFLAGS) $(PROJ_LDFLAGS)

sumo_LDADD   = ./netload/libnetload.a \
./microsim/libmicrosim.a \
./microsim/devices/libmicrosimdevs.a \
./microsim/output/libmicrosimoutput.a \
./microsim/trigger/libmicrosimtrigger.a \
./microsim/output/e1_detectors/libmicrosimoutpute1.a \
./microsim/output/e2_detectors/libmicrosimoutpute2.a \
./microsim/output/e3_detectors/libmicrosimoutpute3.a \
./microsim/output/meandata/libmicrosimoutputmeanoutput.a \
./microsim/actions/libmsactions.a \
./microsim/lanechanging/libmicrosimlanechange.a \
./microsim/traffic_lights/libmicrosimtls.a \
./sumo_only/libsumoonly.a \
./utils/geom/libgeom.a \
./utils/shapes/libshapes.a \
$(COMMON_LIBS) \
$(MEM_LIBS) \
$(ITM_LIBS) \
$(TCPIP_LIBS) \
$(XERCES_LIBS) \
$(PROJ_LIBS)



sumo_netconvert_SOURCES = netconvert_main.cpp

sumo_netconvert_LDFLAGS = $(XERCES_LDFLAGS) $(GDAL_LDFLAGS) $(PROJ_LDFLAGS)

sumo_netconvert_LDADD   = ./netimport/libnetimport.a \
./netimport/cell/libcellimport.a \
./netimport/sumo/libsumoimport.a \
./netimport/visum/libvisumimport.a \
./netimport/xml/libxmlimport.a \
./netimport/arcview/libarcviewimport.a \
./netimport/elmar/libelmarimport.a \
./netimport/elmar2/libelmar2import.a \
./netimport/tiger/libtigerimport.a \
./netimport/vissim/libvissimimport.a \
./netimport/vissim/typeloader/libvissimtypeload.a \
./netimport/vissim/tempstructs/libvissimtmpstr.a \
./netimport/NINavTeqHelper.o \
./netbuild/libnetbuild.a \
./netbuild/nodes/libnodesnetbuild.a \
./utils/distribution/libdistribution.a \
./utils/geom/libgeom.a \
$(COMMON_LIBS) \
$(MEM_LIBS) \
$(TCPIP_LIBS) \
$(XERCES_LIBS) \
$(GDAL_LIBS) \
$(PROJ_LIBS)



sumo_netgen_SOURCES = netgen_main.cpp

sumo_netgen_LDFLAGS = $(XERCES_LDFLAGS) $(PROJ_LDFLAGS)

sumo_netgen_LDADD   = ./netgen/libnetgen.a \
./netbuild/nodes/libnodesnetbuild.a \
./netbuild/libnetbuild.a \
./utils/geom/libgeom.a \
$(COMMON_LIBS) \
$(MEM_LIBS) \
$(XERCES_LIBS) \
$(PROJ_LIBS)



sumo_duarouter_SOURCES = duarouter_main.cpp

sumo_duarouter_LDFLAGS = $(XERCES_LDFLAGS)

sumo_duarouter_LDADD   = ./routing_dua/libduarouting.a \
./router/librouter.a \
./utils/router/librouter.a \
$(COMMON_LIBS) \
$(MEM_LIBS) \
$(XERCES_LIBS)



sumo_dfrouter_SOURCES = dfrouter_main.cpp

sumo_dfrouter_LDFLAGS = $(XERCES_LDFLAGS)

sumo_dfrouter_LDADD   = ./routing_df/libdfrouting.a \
./router/librouter.a \
./utils/router/librouter.a \
./utils/geom/libgeom.a \
$(COMMON_LIBS) \
$(MEM_LIBS) \
$(XERCES_LIBS)



sumo_jtrrouter_SOURCES = jtrrouter_main.cpp

sumo_jtrrouter_LDFLAGS = $(XERCES_LDFLAGS)

sumo_jtrrouter_LDADD   = ./routing_jtr/libjtrrouting.a \
./router/librouter.a \
./utils/router/librouter.a \
$(COMMON_LIBS) \
$(MEM_LIBS) \
$(XERCES_LIBS)



sumo_od2trips_SOURCES = od2trips_main.cpp

sumo_od2trips_LDFLAGS = $(XERCES_LDFLAGS)

sumo_od2trips_LDADD   = ./od2trips/libod2trips.a \
./utils/options/liboptions.a \
./utils/distribution/libdistribution.a \
./utils/geom/libgeom.a \
$(COMMON_LIBS) \
$(MEM_LIBS) \
$(XERCES_LIBS)


sumo_polyconvert_SOURCES = polyconvert_main.cpp

sumo_polyconvert_LDFLAGS = $(XERCES_LDFLAGS) $(PROJ_LDFLAGS)

sumo_polyconvert_LDADD   = ./polyconvert/libpolyconvert.a \
./utils/options/liboptions.a \
./utils/geom/libgeom.a \
./utils/shapes/libshapes.a \
$(COMMON_LIBS) \
$(MEM_LIBS) \
$(XERCES_LIBS) \
$(PROJ_LIBS)


if WITH_GUI
sumo_guisim_SOURCES = guisim_main.cpp

sumo_guisim_LDFLAGS = $(FOX_LDFLAGS) $(XERCES_LDFLAGS) $(X_LDFLAGS)

sumo_guisim_LDADD =  ./gui/libgui.a \
./guinetload/libguinetload.a \
./guisim/libguisim.a \
./gui/drawerimpl/libguidrawerimpl.a \
./gui/dialogs/libguidialogs.a \
./utils/gui/windows/libguiutilsWindows.a \
./utils/gui/globjects/libguiutilsglobjects.a \
./utils/gui/div/libguiutilsdiv.a \
./utils/gui/drawer/libguiutilsdrawer.a \
./utils/gui/events/libguiutilsevents.a \
./utils/gui/images/libguiutilsimages.a \
./utils/gui/tracker/libguiutilstracker.a \
./utils/foxtools/libfoxtools.a \
$(sumo_LDADD) \
./utils/glutils/libglutils.a \
./gui/GUIManipulator.o \
./utils/gui/drawer/GUICompleteSchemeStorage.o \
-l$(LIB_FOX)
endif


SUBDIRS = foreign microsim netbuild netedit netgen netimport netload \
od2trips polyconvert router routing_df routing_dua routing_jtr sumo_only \
tools utils $(GUI_DIRS) $(ITM_DIRS)

EXTRA_DIST = config.h config.h.in \
giant_main.cpp \
guisim.rc \
netedit_main.cpp \
tagreader.cpp \
windows_config.h

if WITH_VERSION_H
all: version.h
version.h: .svn/entries
	../tools/build/version.py
endif
