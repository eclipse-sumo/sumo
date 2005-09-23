if WITH_GUI
GUI_APPS = sumo-guisim
GUI_DIRS = gui guinetload guisim
GUI_INCS = -I$(FOX_INCLUDES)
endif


bin_PROGRAMS = sumo sumo-netconvert sumo-netgen sumo-duarouter sumo-jtrrouter \
sumo-od2trips $(GUI_APPS)

EXTRA_PROGRAMS =

sumo_SOURCES = sumo_main.cpp sumo_help.h \
sumo_version.cpp sumo_version.h sumo_build.h

sumo_LDFLAGS = -L$(XERCES_LDFLAGS)

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
./utils/xml/libxml.a \
./utils/sumoxml/libsumoxml.a \
./utils/gfx/libgfx.a \
./utils/iodevices/libiodevices.a \
-l$(LIB_XERCES)



sumo_netconvert_SOURCES = netconvert_main.cpp netconvert_help.h \
sumo_version.cpp sumo_version.h netconvert_build.h

sumo_netconvert_LDFLAGS = -L$(XERCES_LDFLAGS)

sumo_netconvert_LDADD   = ./netimport/libnetimport.a \
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
-l$(LIB_XERCES)



sumo_netgen_SOURCES = netgen_main.cpp netgen_help.h \
sumo_version.cpp sumo_version.h netgen_build.h

sumo_netgen_LDFLAGS = -L$(XERCES_LDFLAGS)

sumo_netgen_LDADD   = ./netgen/libnetgen.a \
./netbuild/nodes/libnodesnetbuild.a \
./netbuild/libnetbuild.a \
./utils/sumoxml/libsumoxml.a \
./utils/geom/libgeom.a \
./utils/options/liboptions.a \
./utils/common/libcommon.a \
./utils/importio/libimportio.a \
./utils/xml/libxml.a \
-l$(LIB_XERCES)



sumo_duarouter_SOURCES = duarouter_main.cpp duarouter_help.h \
sumo_version.cpp sumo_version.h duarouter_build.h

sumo_duarouter_LDFLAGS = -L$(XERCES_LDFLAGS)

sumo_duarouter_LDADD   = ./routing_dua/libduarouting.a \
./router/librouter.a \
./utils/router/librouter.a \
./utils/options/liboptions.a \
./utils/common/libcommon.a \
./utils/gfx/libgfx.a \
./utils/sumoxml/libsumoxml.a \
./utils/xml/libxml.a \
./utils/importio/libimportio.a \
-l$(LIB_XERCES)



sumo_jtrrouter_SOURCES = jtrrouter_main.cpp jtrrouter_help.h \
sumo_version.cpp sumo_version.h jtrrouter_build.h

sumo_jtrrouter_LDFLAGS = -L$(XERCES_LDFLAGS)

sumo_jtrrouter_LDADD   = ./routing_jtr/libjtrrouting.a \
./router/librouter.a \
./utils/router/librouter.a \
./utils/options/liboptions.a \
./utils/common/libcommon.a \
./utils/gfx/libgfx.a \
./utils/sumoxml/libsumoxml.a \
./utils/xml/libxml.a \
./utils/importio/libimportio.a \
-l$(LIB_XERCES)



sumo_od2trips_SOURCES = od2trips_main.cpp od2trips_help.h \
sumo_version.cpp sumo_version.h od2trips_build.h

sumo_od2trips_LDFLAGS = -L$(XERCES_LDFLAGS)

sumo_od2trips_LDADD   = ./od2trips/libod2trips.a \
./utils/sumoxml/libsumoxml.a \
./utils/options/liboptions.a \
./utils/common/libcommon.a \
./utils/importio/libimportio.a \
./utils/xml/libxml.a \
-l$(LIB_XERCES)



if WITH_GUI
sumo_guisim_SOURCES = guisim_main.cpp guisim_help.h \
sumo_version.cpp sumo_version.h guisim_build.h

sumo_guisim_LDFLAGS = -L$(FOX_LDFLAGS) -L$(XERCES_LDFLAGS) $(X_LDFLAGS)

sumo_guisim_LDADD =  ./gui/libgui.a \
./guinetload/libguinetload.a \
./guisim/libguisim.a \
./gui/drawerimpl/libguidrawerimpl.a \
./gui/dialogs/libguidialogs.a \
./gui/GUISupplementaryWeightsHandler.o \
./utils/foxtools/libfoxtools.a \
./utils/gui/div/libguiutilsdiv.a \
./utils/gui/drawer/libguiutilsdrawer.a \
./utils/gui/events/libguiutilsevents.a \
./utils/gui/images/libguiutilsimages.a \
./utils/gui/tracker/libguiutilstracker.a \
./utils/gui/windows/libguiutilsWindows.a \
./utils/gui/globjects/libguiutilsglobjects.a \
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
./utils/xml/libxml.a \
./utils/gfx/libgfx.a \
./utils/glutils/libglutils.a \
./utils/iodevices/libiodevices.a \
./gui/GUIManipulator.o \
-l$(LIB_XERCES) \
-l$(LIB_FOX) \
$(LIB_X11)
endif



AM_CPPFLAGS = -I$(XERCES_INCLUDES) -I$(XERCES_INCLUDES_PRIOR_1_7) \
$(GUI_INCS)


SUBDIRS = microsim netload sumo_only \
netbuild netgen netimport \
od2trips router routing_dua routing_jtr \
utils $(GUI_DIRS)

EXTRA_DIST = sumo_version.cpp sumo_version.h \
giant_help.h giant_main.cpp netedit_help.h netedit_main.cpp \
tagreader.cpp



