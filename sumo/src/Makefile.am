bin_PROGRAMS = sumo \
sumo-netconvert \
sumo-netgen \
sumo-router \
sumo-od2trips \
sumo-gui \
tagreader


sumo_SOURCES = sumo_main.cpp \
sumo_help.h
sumo_LDFLAGS = -L$(XERCES_LDFLAGS)
sumo_LDADD   = ./netload/libnetload.a \
./microsim/libmicrosim.a \
./utils/common/libcommon.a \
./utils/convert/libconvert.a \
./utils/options/liboptions.a \
./utils/xml/libxml.a \
./utils/sumoxml/libsumoxml.a \
./sumo_only/libsumoonly.a \
./helpers/libhelpers.a \
-l$(LIB_XERCES)


sumo_netconvert_SOURCES = netconvert_main.cpp \
netconvert_help.h
sumo_netconvert_LDFLAGS = -L$(XERCES_LDFLAGS)
sumo_netconvert_LDADD   = ./netimport/libnetimport.a \
./netimport/cell/libcellimport.a \
./netimport/sumo/libsumoimport.a \
./netimport/visum/libvisumimport.a \
./netimport/xml/libxmlimport.a \
./netimport/arcview/libarcviewimport.a \
./netimport/artemis/libartemisimport.a \
./netimport/vissim/libvissimimport.a \
./netimport/vissim/typeloader/libvissimtypeload.a \
./netimport/vissim/tempstructs/libvissimtmpstr.a \
./netbuild/libnetbuild.a \
./utils/sumoxml/libsumoxml.a \
./utils/distribution/libdistribution.a \
./utils/geom/libgeom.a \
./utils/gfx/libgfx.a \
./utils/options/liboptions.a \
./utils/common/libcommon.a \
./utils/importio/libimportio.a \
./utils/convert/libconvert.a \
./utils/xml/libxml.a \
-l$(LIB_XERCES)


#sumo_netgen_SOURCES = netgen_main.cpp \
#netgen_help.h
#sumo_netgen_LDFLAGS = -L$(XERCES_LDFLAGS)
#sumo_netgen_LDADD   = ./netgen/libnetgen.a \
#./netbuild/libnetbuild.a \
#./utils/sumoxml/libsumoxml.a \
#./utils/geom/libgeom.a \
#./utils/options/liboptions.a \
#./utils/common/libcommon.a \
#./utils/importio/libimportio.a \
#./utils/convert/libconvert.a \
#./utils/xml/libxml.a \
#-l$(LIB_XERCES)


sumo_router_SOURCES = router_main.cpp \
router_help.h
sumo_router_LDFLAGS = -L$(XERCES_LDFLAGS)
sumo_router_LDADD   = ./router/librouter.a \
./utils/options/liboptions.a \
./utils/convert/libconvert.a \
./utils/common/libcommon.a \
./utils/gfx/libgfx.a \
./utils/sumoxml/libsumoxml.a \
./utils/xml/libxml.a \
./utils/router/librouter.a \
./utils/importio/libimportio.a \
-l$(LIB_XERCES)


sumo_od2trips_SOURCES = od2trips_main.cpp \
od2trips_help.h
sumo_od2trips_LDFLAGS = -L$(XERCES_LDFLAGS)
sumo_od2trips_LDADD   = ./od2trips/libod2trips.a \
./utils/sumoxml/libsumoxml.a \
./utils/options/liboptions.a \
./utils/convert/libconvert.a \
./utils/common/libcommon.a \
./utils/xml/libxml.a \
-l$(LIB_XERCES)


sumo_gui_SOURCES = gui_main.cpp \
gui_help.h \
sumo_version.cpp \
sumo_version.h
sumo_gui_LDFLAGS = -L$(XERCES_LDFLAGS) $(QT_LDFLAGS) $(X_LDFLAGS)
sumo_gui_LDADD =  ./gui/libgui.a \
./guinetload/libguinetload.a \
./guisim/libguisim.a \
./gui/popup/libguipopup.a \
./gui/partable/libguipartable.a \
./gui/vartracker/libguivartracker.a \
./netload/libnetload.a \
./microsim/libmicrosim.a \
./utils/common/libcommon.a \
./utils/xml/libxml.a \
./utils/sumoxml/libsumoxml.a \
./utils/convert/libconvert.a \
./utils/options/liboptions.a \
./sumo_only/libsumoonly.a \
./helpers/libhelpers.a \
./utils/geom/libgeom.a \
./utils/fonts/libfonts.a \
./utils/gfx/libgfx.a \
./utils/qutils/libqutils.a \
./utils/glutils/libglutils.a \
-l$(LIB_XERCES) \
$(LIB_QT) \
$(LIB_X11)

tagreader_SOURCES = tagreader.cpp
tagreader_LDFLAGS = $(QT_LDFLAGS) $(X_LDFLAGS)
tagreader_LDADD   = ./div/libdiv.a \
$(LIB_QT) \
$(LIB_X11)



AM_CPPFLAGS = -Imicrosim \
-Ihelpers \
-I$(XERCESINCLUDE) \
-I$(XERCESINCLUDE_PRIOR_1_7) \
$(QT_INCLUDES)



SUBDIRS = div \
gui \
guinetload \
guisim \
helpers \
microsim \
netbuild \
netimport \
netload \
od2trips \
router \
sumo_only \
utils

EXTRA_DIST = sumo_version.cpp sumo_version.h

