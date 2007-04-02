if WITH_GUI
GUI_APPS = sumo-guisim
GUI_DIRS = gui guinetload guisim icons
endif

if CHECK_MEMORY_LEAKS
MEM_LIBS  = ./foreign/nvwa/libnvwa.a
endif

if ITM
ITM_LIBS = ./itm-remoteserver/libitmremoteserver.a
ITM_DIRS = itm-remoteserver
endif

bin_PROGRAMS = sumo sumo-netconvert sumo-netgen \
sumo-duarouter sumo-dfrouter sumo-jtrrouter \
sumo-od2trips sumo-polyconvert $(GUI_APPS)

EXTRA_PROGRAMS =

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
./utils/bindevice/libbindevice.a \
./utils/common/libcommon.a \
./utils/options/liboptions.a \
./utils/sumoxml/libsumoxml.a \
./utils/xml/libxml.a \
./utils/gfx/libgfx.a \
./utils/iodevices/libiodevices.a \
./utils/common/SUMOVehicleClass.o \
./utils/geoconv/libgeoconv.a \
$(MEM_LIBS) \
$(ITM_LIBS) \
-l$(LIB_GDAL) -l$(LIB_XERCES) -l$(LIB_PROJ)



sumo_netconvert_SOURCES = netconvert_main.cpp

sumo_netconvert_LDFLAGS = $(XERCES_LDFLAGS) $(GDAL_LDFLAGS) $(PROJ_LDFLAGS)

sumo_netconvert_LDADD   = ./netimport/libnetimport.a \
./utils/geoconv/libgeoconv.a \
./netimport/cell/libcellimport.a \
./netimport/sumo/libsumoimport.a \
./netimport/visum/libvisumimport.a \
./netimport/xml/libxmlimport.a \
./netimport/arcview/libarcviewimport.a \
./netimport/artemis/libartemisimport.a \
./netimport/elmar/libelmarimport.a \
./netimport/elmar2/libelmar2import.a \
./netimport/tiger/libtigerimport.a \
./netimport/vissim/libvissimimport.a \
./netimport/vissim/typeloader/libvissimtypeload.a \
./netimport/vissim/tempstructs/libvissimtmpstr.a \
./netimport/NINavTeqHelper.o \
./netbuild/libnetbuild.a \
./netbuild/nodes/libnodesnetbuild.a \
./utils/sumoxml/libsumoxml.a \
./utils/distribution/libdistribution.a \
./utils/geom/libgeom.a \
./utils/gfx/libgfx.a \
./utils/options/liboptions.a \
./utils/common/libcommon.a \
./utils/importio/libimportio.a \
./utils/xml/libxml.a \
$(MEM_LIBS) \
-l$(LIB_GDAL) -l$(LIB_XERCES) -l$(LIB_PROJ)



sumo_netgen_SOURCES = netgen_main.cpp

sumo_netgen_LDFLAGS = $(XERCES_LDFLAGS) $(PROJ_LDFLAGS)

sumo_netgen_LDADD   = ./netgen/libnetgen.a \
./utils/geoconv/libgeoconv.a \
./netbuild/nodes/libnodesnetbuild.a \
./netbuild/libnetbuild.a \
./utils/sumoxml/libsumoxml.a \
./utils/geom/libgeom.a \
./utils/options/liboptions.a \
./utils/common/libcommon.a \
./utils/importio/libimportio.a \
./utils/xml/libxml.a \
$(MEM_LIBS) \
-l$(LIB_XERCES) -l$(LIB_PROJ)



sumo_duarouter_SOURCES = duarouter_main.cpp

sumo_duarouter_LDFLAGS = $(XERCES_LDFLAGS)

sumo_duarouter_LDADD   = ./routing_dua/libduarouting.a \
./router/librouter.a \
./utils/router/librouter.a \
./utils/options/liboptions.a \
./utils/common/libcommon.a \
./utils/gfx/libgfx.a \
./utils/sumoxml/libsumoxml.a \
./utils/xml/libxml.a \
./utils/importio/libimportio.a \
$(MEM_LIBS) \
-l$(LIB_XERCES)



sumo_dfrouter_SOURCES = dfrouter_main.cpp

sumo_dfrouter_LDFLAGS = $(XERCES_LDFLAGS)

sumo_dfrouter_LDADD   = ./routing_df/libdfrouting.a \
./router/librouter.a \
./utils/router/librouter.a \
./utils/options/liboptions.a \
./utils/geom/libgeom.a \
./utils/common/libcommon.a \
./utils/gfx/libgfx.a \
./utils/sumoxml/libsumoxml.a \
./utils/xml/libxml.a \
./utils/importio/libimportio.a \
$(MEM_LIBS) \
-l$(LIB_XERCES)



sumo_jtrrouter_SOURCES = jtrrouter_main.cpp

sumo_jtrrouter_LDFLAGS = $(XERCES_LDFLAGS)

sumo_jtrrouter_LDADD   = ./routing_jtr/libjtrrouting.a \
./router/librouter.a \
./utils/router/librouter.a \
./utils/options/liboptions.a \
./utils/common/libcommon.a \
./utils/gfx/libgfx.a \
./utils/sumoxml/libsumoxml.a \
./utils/xml/libxml.a \
./utils/importio/libimportio.a \
$(MEM_LIBS) \
-l$(LIB_XERCES)



sumo_od2trips_SOURCES = od2trips_main.cpp

sumo_od2trips_LDFLAGS = $(XERCES_LDFLAGS)

sumo_od2trips_LDADD   = ./od2trips/libod2trips.a \
./utils/sumoxml/libsumoxml.a \
./utils/options/liboptions.a \
./utils/distribution/libdistribution.a \
./utils/geom/libgeom.a \
./utils/common/libcommon.a \
./utils/importio/libimportio.a \
./utils/gfx/libgfx.a \
./utils/xml/libxml.a \
$(MEM_LIBS) \
-l$(LIB_XERCES)


sumo_polyconvert_SOURCES = polyconvert_main.cpp

sumo_polyconvert_LDFLAGS = $(XERCES_LDFLAGS) $(PROJ_LDFLAGS)

sumo_polyconvert_LDADD   = ./utils/geoconv/libgeoconv.a \
./polyconvert/libpolyconvert.a \
./utils/sumoxml/libsumoxml.a \
./utils/options/liboptions.a \
./utils/geom/libgeom.a \
./utils/common/libcommon.a \
./utils/importio/libimportio.a \
./utils/gfx/libgfx.a \
./utils/xml/libxml.a \
./utils/shapes/libshapes.a \
$(MEM_LIBS) \
-l$(LIB_XERCES) -l$(LIB_PROJ)


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
./netload/libnetload.a \
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
./utils/sumoxml/libsumoxml.a \
./utils/options/liboptions.a \
./sumo_only/libsumoonly.a \
./utils/bindevice/libbindevice.a \
./utils/geom/libgeom.a \
./utils/shapes/libshapes.a \
./utils/common/libcommon.a \
./utils/importio/libimportio.a \
./utils/xml/libxml.a \
./utils/gfx/libgfx.a \
./utils/glutils/libglutils.a \
./utils/iodevices/libiodevices.a \
./gui/GUIManipulator.o \
./utils/gui/drawer/GUICompleteSchemeStorage.o \
./utils/geoconv/libgeoconv.a \
$(MEM_LIBS) \
-l$(LIB_XERCES) \
-l$(LIB_FOX) \
$(LIB_X11) -l$(LIB_PROJ)
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

